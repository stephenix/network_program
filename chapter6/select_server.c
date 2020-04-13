#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>     // connect,listen,accept
#include <netinet/in.h>

#define LISTENQ 10
#define MAXLINE 20
#define SERV_PORT 9877 
int main() {
    int listenfd, connfd;
    pid_t childpid;
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;
    int client[FD_SETSIZE], nready;
    int maxfd, maxi, i, sockfd, n;
    fd_set rset, allset;
    char buf[MAXLINE];

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    listen(listenfd, LISTENQ);

    maxfd = listenfd;
    maxi = -1;
    for (i = 0; i < FD_SETSIZE; i ++)
        client[i] = -1;
    FD_ZERO(&allset);
    FD_SET(listenfd, &allset);

    while (1) {
        // go back initial state in every loop 
        rset = allset;
        nready = select(maxfd + 1, &rset, NULL, NULL, NULL);

        // new client connection
        if (FD_ISSET(listenfd, &rset)) {
            clilen = sizeof(cliaddr);
            connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);

            // set client[] to save cur connfd
            for (i = 0; i < FD_SETSIZE; i ++) {
                if (client[i] < 0) {
                    client[i] = connfd; 
                    break;
                }
            }
            if (i == FD_SETSIZE) {
                perror("too many clients.\n");
                return -1;
            }
            // set allset not rset?
            FD_SET(connfd, &allset);
            // for select
            if (connfd > maxfd)
                maxfd = connfd;
            // max index in client[] array
            if (i > maxi)
                maxi = i;
            // no more readable descriptors
            if (-- nready <= 0)
                continue;
        }

        // check all clients for data
        for (i = 0; i <= maxi; i ++) {
            if ((sockfd = client[i]) < 0)
                continue;
            if (FD_ISSET(sockfd, &rset)) {
                // read eof
                if ((n = read(sockfd, buf, MAXLINE)) == 0) {
                    // connection closed by client
                    close(sockfd);
                    FD_CLR(sockfd, &allset);
                    client[i] = -1;
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

