# IO Multiplexing

## 一、epoll

**Introduce:**

```c
#include <sys/epoll.h>
int epoll_create(int size)；
int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event)；
int epoll_wait(int epfd, struct epoll_event * events, int maxevents, int timeout);
```

```c
int epoll_create(int size)；
创建一个epoll的句柄
```

```c
int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event)；
对指定fd执行op操作
epfd：epoll_create()的返回值
op：表示op操作，用三个宏来表示：添加EPOLL_CTL_ADD，删除EPOLL_CTL_DEL，修改EPOLL_CTL_MOD。分别添加、删除和修改对fd的监听事件
fd:要监听的fd
epoll_event：是告诉内核需要监听什么事
struct epoll_event {
  __uint32_t events;  /* Epoll events */
  epoll_data_t data;  /* User data variable */
};
```

//events可以是以下几个宏的集合：
EPOLLIN ：表示对应的文件描述符可以读（包括对端SOCKET正常关闭）；
EPOLLOUT：表示对应的文件描述符可以写；
EPOLLPRI：表示对应的文件描述符有紧急的数据可读（这里应该表示有带外数据到来）；
EPOLLERR：表示对应的文件描述符发生错误；
EPOLLHUP：表示对应的文件描述符被挂断；
EPOLLET： 将EPOLL设为边缘触发(Edge Triggered)模式，这是相对于水平触发(Level Triggered)来说的。
EPOLLONESHOT：只监听一次事件，当监听完这次事件之后，如果还需要继续监听这个socket的话，需要再次把这个socket加入到EPOLL队列里

```c
int epoll_wait(int epfd, struct epoll_event * events, int maxevents, int timeout);
等待epfd上的io事件，最多返回maxevents个事件
events:表示从内核得到事件的集合
maxevents：告诉内核这个events有多大
返回值：要处理的事件数目
```

**Detail：**

epoll对文件描述符的操作有两种模式：**LT（level trigger）**和**ET（edge trigger）**。LT模式是默认模式，LT模式与ET模式的区别如下：

- LT模式：当epoll_wait检测到描述符事件发生并将此事件通知应用程序，`应用程序可以不立即处理该事件`。下次调用epoll_wait时，会再次响应应用程序并通知此事件。
- ET模式：当epoll_wait检测到描述符事件发生并将此事件通知应用程序，`应用程序必须立即处理该事件`。如果不处理，下次调用epoll_wait时，不会再次响应应用程序并通知此事件。

**Attention:** 

1. select_client.c 

must bzero receive buffer, before next receiving
because next receive buffer length may be shorter than last receive buffer.

2. select_server.c

the role of allset and rset

rset is changed by select, so need allset keep set state.

**Reference：**

[1] https://segmentfault.com/a/1190000003063859

[2] https://www.cnblogs.com/Anker/archive/2013/08/17/3263780.html

## 二、比较

epoll利用内核发现事件准备就绪，callback通知，epoll_wait收到通知，避免select/poll，进程遍历查找准备就绪的事件。





https://cloud.tencent.com/developer/article/1005481

https://www.cnblogs.com/anker/p/3263780.html