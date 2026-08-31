#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <fcntl.h>
using namespace std;

#define MAX_EVENTS  2048
#define PORT        8888
#define BUFF_SIZE   2048

int setListen()
{
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd == -1)
    {
        cout << "create listen_fd faild" << endl;
        exit(-1);
    }

    // allow reuse ip
    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(listen_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
    {
        cout << "bind error" << endl;
        close(listen_fd);
        exit(-1);
    }

    if (listen(listen_fd, SOMAXCONN) == -1)
    {
        cout << "listen error" << endl;
        close(listen_fd);
        exit(-1);
    }

    return listen_fd;
}

int setNoBlock(int listen_fd)
{
    //set noblock
    int flag = fcntl(listen_fd, F_GETFL, 0);
    if (flag == -1)
    {
        cout << "set noblock error" << endl;
        exit(-1);
    }

    if (fcntl(listen_fd, F_SETFL, flag | O_NONBLOCK) == -1)
    {
        cout << "set noblock error" << endl;
        exit(-1);
    }

    return listen_fd;
}

int main()
{
    int listen_fd = setListen();
    listen_fd = setNoBlock(listen_fd);
    //创建epoll
    int epoll_fd = epoll_create(1); //requre > 0
    if (epoll_fd == -1)
    {
        cout << "epoll create faild" << endl;
        close(listen_fd);
        exit(-1);
    }

    //listen_fd add to epoll
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = listen_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &ev) == -1)
    {
        cout << "epoll ctl error" << endl;
        close(listen_fd);
        close(epoll_fd);
        exit(-1);
    }

    struct epoll_event events[MAX_EVENTS];
    cout << "epoll run port" << PORT << endl;
    while (1)
    {
        int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (nfds == -1)
        {
            cout << "epoll wait error" << endl;
            break;
        }
        for (int i = 0; i < nfds; i++)
        {
            int fd = events[i].data.fd;
            if (fd == listen_fd)    //deal new client
            {
                struct sockaddr_in client_addr;
                socklen_t client_len = sizeof(client_addr);
                int client_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &client_len);
                if (client_fd == -1)
                {
                    cout << "accept error" << endl;
                    continue;
                }

                //client socket add to epoll
                ev.events = EPOLLIN;
                ev.data.fd = client_fd;
                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) == -1)
                {
                    cout << "epoll add fd error" << endl;
                    close(client_fd);
                    continue;
                }
                printf("add new connection form: %s , %d, fd = %d \n", \
                    inet_ntoa(client_addr.sin_addr), \
                    ntohs(client_addr.sin_port),\
                    client_fd
                );
            }
            else    //deal exist client
            {
                char buff[BUFF_SIZE];
                buff[0] = {'\0'};
                ssize_t len = read(fd, buff, BUFF_SIZE - 1);
                if (len > 0)
                {
                    buff[len] = '\0';
                    cout << "recv :" << len << "bytes form fd:" << fd << "datas:" << buff << endl;
                    // rewrite data to client
                    ssize_t written = write(fd, buff, len);
                    if (written == -1)
                    {
                        cout << "write error" << endl;
                        close(fd);
                        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
                    }
                }
                else if (len == 0)
                {
                    //client close link
                    cout << "client close fd:" << fd << endl;
                    close(fd);
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
                }
                else 
                {
                    //read error
                    if (errno != EAGAIN)
                    {
                        cout << "read error" << endl;
                        close(fd);
                        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
                    }
                }
            }
        }
    }

    close(listen_fd);
    close(epoll_fd);
    return 0;
}