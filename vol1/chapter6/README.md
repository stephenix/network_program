# IO Multiplexing

## 一、select

**Introduce:**

```c
include <sys/select.h>
int select(int nfds, fd_set *readfds, fd_set *writefds,
        fd_set *exceptfds, struct timeval *timeout);
nfds: this argument should be set to the highest-numbered file
        descriptor in any of the three sets, plus 1.
        感兴趣的文件描述的最大值+1
返回值: 有多少事件准备好
```

**Attention:** 

1. select_client.c 

must bzero receive buffer, before next receiving
because next receive buffer length may be shorter than last receive buffer.

2. select_server.c

the role of allset and rset

rset is changed by select, so need allset keep set state.

**Reference：**

[1] 《UNIX网络编程:卷1》
[2] https://www.cnblogs.com/webor2006/p/4055284.html

## 二、poll

**Introduce:**

```c
include <poll.h>
int poll(struct pollfd *fds, nfds_t nfds, int timeout);

struct pollfd {
	int fd;                 /* 文件描述符 /
	short events;           / 等待的事件 /
	short revents;          / 实际发生了的事件 */
};
```



## 三、epoll

