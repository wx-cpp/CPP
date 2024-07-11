#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/fcntl.h>
#include <sys/epoll.h>

// 初始化服务端的监听端口
int initserver(int port)
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("socked failed");
        return -1;
    }

    int opt = 1;
    unsigned int len = sizeof(opt);
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, len);

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);

    // 绑定
    if (bind(sockfd, (sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("bind failed");
        close(sockfd);
        return -1;
    }

    // 监听
    if (listen(sockfd, 10) != 0)
    {
        perror("listen failed");
        close(sockfd);
        return -1;
    }
    return sockfd;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("输入./server_epoll port");
        return -1;
    }

    // 初始化服务端用于监听的socket
    int listen_sock = initserver(atoi(argv[1]));
    printf("listen_sock=%d\n", listen_sock);
    if (listen_sock < 0)
    {
        perror("listen failed");
        return -1;
    }

    // 创建epoll句柄，标识监听最大数目
    int epollfd = epoll_create(100);
    if (epollfd < 0)
    {
        perror("epoll creat failed");
        return -1;
    }

    // 创建监视fd列表
    epoll_event server_fd_event;
    // 检测事件初始化
    server_fd_event.data.fd = listen_sock;
    server_fd_event.events = EPOLLIN;

    // 将监视的socket和事件加入到epollfd中
    epoll_ctl(epollfd, EPOLL_CTL_ADD, listen_sock, &server_fd_event);

    // 存放epoll返回的事件
    epoll_event events[1024];

    int s1 = sizeof(events);
    int s2 = sizeof(events[0]);

    char send_buf[1024];
    int send_len = sizeof(send_buf);
    memset(send_buf, 0, send_len);

    while (true)
    {
        // 等待监视的socket有事件发生
        int infds = epoll_wait(epollfd, events, 1024, -1);

        // 返回失败
        if (infds < 0)
        {
            perror("epoll failed\n");
            break;
        }
        // 超时
        if (infds == 0)
        {
            perror("epoll timeout\n");
            continue;
        }

        for (int i = 0; i < infds; i++)
        {
            // 有新客户端连接
            if (events[i].data.fd == listen_sock)
            {
                sockaddr_in client;
                socklen_t len = sizeof(client);
                int clientfd = accept(listen_sock, (sockaddr *)&client, &len);
                printf("连接客户端(socket=%d)成功\n", clientfd);

                // 给新客户端准备读事件，添加到epoll中
                server_fd_event.data.fd = clientfd;
                server_fd_event.events = EPOLLIN;
                epoll_ctl(epollfd, EPOLL_CTL_ADD, clientfd, &server_fd_event);
            }
            else
            {
                // 有读写事件触发或断开连接
                char buffer[1024];
                memset(buffer, 0, sizeof(buffer));
                if (recv(events[i].data.fd, buffer, sizeof(buffer), 0) <= 0)
                {
                    // 断开连接
                    printf("客户端(eventfd=%d)已断开连接\n", events[i].data.fd); // 标准输出通常是行缓冲，若没有换行符则不会立即打印出输出
                    close(events[i].data.fd);
                    epoll_ctl(epollfd, EPOLL_CTL_DEL, events[i].data.fd, 0);
                }
                else
                {
                    printf("接收到(eventfd=%d):%s\n", events[i].data.fd, buffer);
                    send(events[i].data.fd, buffer, strlen(buffer), 0);
                }
            }
        }
    }

    return 0;
}
