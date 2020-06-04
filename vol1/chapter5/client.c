#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAXLINE 100
#define SERV_PORT 9877 
void str_cli(FILE *fp, int sockfd);
int main(int argc, char** argv) {
    int sockfd;
    struct sockaddr_in servaddr;
    int status = -1;

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    //inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);
    inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    status = connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    if (status < 0) {
	    printf("connect server error.\n");
	    return -1;
    }
    str_cli(stdin, sockfd);

    return 0;
}

void str_cli(FILE *fp, int sockfd) {
    char send[MAXLINE], recieve[MAXLINE];
    while (fgets(send, MAXLINE, fp) != NULL) {
        write(sockfd, send, strlen(send));
    	read(sockfd, recieve, MAXLINE);
    	fputs(recieve, stdout);
        // must initial again, or echo undefined
	    bzero(recieve, MAXLINE);
    }
}
