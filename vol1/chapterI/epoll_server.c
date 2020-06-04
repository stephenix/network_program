#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <sys/epoll.h>
#include <sys/socket.h>     // connect,listen,accept
#include <netinet/in.h>
#include <arpa/inet.h>

#define LISTENQ 10
#define MAXLINE 20
#define IPADDRESS "127.0.0.1" 
#define SERV_PORT 9877 
#define FD_SIZE 1000
#define EPOLLEVENTS 100
// create socket and bind
static int socket_bind(const char *ip, int port);
// begin epoll
static void do_epoll(int listenfd);
static void handle_events(int epollfd, struct epoll_event *events, int nready, int listenfd, char *buf);
static void handle_accept(int epollfd, int listenfd);
static void handle_read(int epollfd, int fd, char *buf);
static void handle_write(int epollfd, int fd, char *buf);
static void add_event(int epollfd, int fd, int state);
static void delete_event(int epollfd, int fd, int state);
static void modify_event(int epollfd, int fd, int state);

int main() {
    int listenfd;
    listenfd = socket_bind(IPADDRESS, SERV_PORT);
    listen(listenfd, LISTENQ);
    do_epoll(listenfd);

    return 0;
}

static int socket_bind(const char *ip, int port) {
    int listenfd;
    socklen_t clilen;
    struct sockaddr_in servaddr;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1) {
        perror("socket error.");
        exit(1);
    }
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    //servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    inet_pton(AF_INET, ip, &servaddr.sin_addr);
    servaddr.sin_port = htons(SERV_PORT);

    if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
        perror("socket error.");
        exit(1);
    }
    return listenfd;
}

static void do_epoll(int listenfd) {
    int epollfd;
    struct epoll_event events[EPOLLEVENTS];
    int nready;
    char buf[MAXLINE];

    memset(buf, 0, MAXLINE);
    epollfd = epoll_create(FD_SIZE);
    add_event(epollfd, listenfd, EPOLLIN);

    for (; ;) {
        nready = epoll_wait(epollfd, events, EPOLLEVENTS, -1); 
        handle_events(epollfd, events, nready, listenfd, buf);
    }
    close(epollfd);
}

static void handle_events(int epollfd, struct epoll_event *events, int nready, int listenfd, char *buf) {
    int i;
    int fd;

    for (i = 0; i < nready; i ++) {
        fd = events[i].data.fd;
        if ((fd == listenfd) && (events[i].events & EPOLLIN)) {
            handle_accept(epollfd, listenfd);
        } else if (events[i].events & EPOLLIN) {
            handle_read(epollfd, fd, buf);
        } else if (events[i].events & EPOLLOUT) {
            handle_write(epollfd, fd, buf);
        }
    }
}

static void handle_accept(int epollfd, int listenfd) {
    int clifd;
    struct sockaddr_in cliaddr;
    socklen_t cliaddrlen;

    clifd = accept(listenfd, (struct sockaddr*)&cliaddr, &cliaddrlen);
    if (clifd == -1) {
        perror("accept error.");
    } else {
        // add monitor read event 
        add_event(epollfd, clifd, EPOLLIN);
    }
}
static void handle_read(int epollfd, int fd, char *buf) {
    int nread;
    nread = read(fd, buf, MAXLINE);
    if (nread == -1) {
        perror("read error.");
        close(fd);
        delete_event(epollfd, fd, EPOLLIN);
    } else if (nread == 0){
        fprintf(stderr, "client error.\n");
        close(fd);
        delete_event(epollfd, fd, EPOLLIN);
    } else {
        printf("read message: %s", buf);
        modify_event(epollfd, fd, EPOLLOUT);
    }
}
static void handle_write(int epollfd, int fd, char *buf) {
    int nwrite;
    nwrite = write(fd, buf, strlen(buf));
    if (nwrite == -1) {
        perror("write error.");
        close(fd);
        delete_event(epollfd, fd, EPOLLOUT);
    } else {
        modify_event(epollfd, fd, EPOLLIN);
    }
    memset(buf, 0, MAXLINE);
}

static void add_event(int epollfd, int fd, int state) {
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
}

static void delete_event(int epollfd, int fd, int state) {
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &ev);
}

static void modify_event(int epollfd, int fd, int state) {
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &ev);
}
