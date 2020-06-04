#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <poll.h>
#include <sys/socket.h>     // connect,listen,accept
#include <netinet/in.h>

#define LISTENQ 10
#define MAXLINE 20
#define SERV_PORT 9877 
#define INFTIM -1
int main() {
    int listenfd, connfd;
    pid_t childpid;
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;
    struct pollfd client[FOPEN_MAX];
    int maxfd, maxi, i, sockfd, n, nready;
    fd_set rset, allset;
    char buf[MAXLINE];

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    listen(listenfd, LISTENQ);

    client[0].fd = listenfd; 
    client[0].events = POLLRDNORM;
    for (i = 1; i < FOPEN_MAX; i ++)
        client[i].fd = -1;
    // max index into client[] array
    maxi = 0;

    while (1) {
        // INFTIM是永远等待
        nready = poll(client, maxi + 1, INFTIM);

        // new client connection
        if (client[0].revents & POLLRDNORM) {
            clilen = sizeof(cliaddr);
            connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);

            // set client[] to save cur connfd
            for (i = 1; i < FOPEN_MAX; i ++) {
                if (client[i].fd < 0) {
                    client[i].fd = connfd; 
                    break;
                }
            }
            if (i == FOPEN_MAX) {
                perror("too many clients.\n");
                return -1;
            }
            // set interest event 
            client[i].events = POLLRDNORM;

            // max index in client[] array
            if (i > maxi)
                maxi = i;
            // no more readable descriptors
            if (-- nready <= 0)
                continue;
        }

        // check all clients for data
        for (i = 1; i <= maxi; i ++) {
            if ((sockfd = client[i].fd) < 0)
                continue;
            if (client[i].revents & (POLLRDNORM | POLLERR)) {
                // read eof
                if ((n = read(sockfd, buf, MAXLINE)) < 0) {
                    // connection reset by client
                    if (errno == ECONNRESET) {
                        close(sockfd);
                        client[i].fd = -1;
                    } else {
                        perror("read error.\n");
                        return -1;
                    }
                } else if (n == 0) {
                    // connection closed by client
                    close(sockfd);
                    client[i].fd = -1;
                } else {
                    write(sockfd, buf, n);
                }
                // maybe multiple ready fds
                // no more readable descriptors
                if (-- nready <= 0)
                    break;
            }
        }
    }
    
    return 0;
}

