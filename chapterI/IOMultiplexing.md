

select、poll、epoll都是IO复用的一种方法。

1.为什么用？适用场景？

因为IO复用可以用较少的代价同时监听处理多个IO。

- 当客户处理多个描述字时（一般是交互式输入和网络套接口），必须使用I/O复用。 
- 当一个客户同时处理多个套接口时，而这种情况是可能的，但很少出现。 
- 如果一个TCP服务器既要处理监听套接口，又要处理已连接套接口，一般也要用到I/O复用。 
- 如果一个服务器即要处理TCP，又要处理UDP，一般要使用I/O复用。 
- 如果一个服务器要处理多个服务或多个协议，一般要使用I/O复用。

2.IO多路复用是同步IO

因为当事件就绪后，仍然需要自己负责读写，读写过程是阻塞，异步IO则不是。

非阻塞和异步的区别，非阻塞只是第一阶段不阻塞，异步是两个阶段都不阻塞。

3.本质是什么？



4.如何选择？

select缺点：

（1）每次需要从用户态拷贝到内核态，性能损耗大，为此限制监听fd数量

（2）每次需要遍历整个fds，即使只有一个事件就绪

（3）为了降低性能损耗，内核限制监听的fd_set集合中socket数量为1024

poll缺点：poll解决select的缺点1，把fd_set修改成数组strcut pollfd client[] 

epoll缺点：















Reference：

[1] https://cloud.tencent.com/developer/article/1005481

[2] https://www.iminho.me/wiki/blog-23.html

[3] https://ruby-china.org/topics/39644

