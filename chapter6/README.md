## Description:
#include <sys/select.h>
int select(int nfds, fd_set *readfds, fd_set *writefds,
        fd_set *exceptfds, struct timeval *timeout);
nfds: this argument should be set to the highest-numbered file
        descriptor in any of the three sets, plus 1.
        感兴趣的文件描述的最大值+1

## Attention: 
1. select_client.c 
must bzero receive buffer, before next receiving
because next receive buffer length may be shorter than last receive buffer.
2. select_server.c
the role of allset and rset
rset is changed by select, so need allset keep set state.


## reference
[1] https://www.cnblogs.com/webor2006/p/4055284.html
