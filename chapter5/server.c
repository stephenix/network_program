#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>     // connect,listen,accept
#include <netinet/in.h>

#define LISTENQ 10
#define MAXLINE 20
#define SERV_PORT 9877 
void str_echo(int);
int main() {
    int listenfd, connfd;
    pid_t childpid;
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    listen(listenfd, LISTENQ);

    while (1) {
        clilen = sizeof(cliaddr);
        connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);
        // fork child process to handle
        if ((childpid = fork()) == 0) {
            // close listenfd 
            close(listenfd);
            str_echo(connfd);
            // close connfd
            close(connfd);
            return 0;
        }
        // parent process: close connfd
        close(connfd);
    }
    
    return 0;
}

void str_echo(int sockfd) {
    size_t n;
    char buf[MAXLINE];

    while ((n = read(sockfd, buf, MAXLINE)) > 0) {
        // size must be n, or echo undefined 
        write(sockfd, buf, n);
    }
}
