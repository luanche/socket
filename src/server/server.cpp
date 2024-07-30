#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/poll.h>
#include <sys/epoll.h>
#include "server.h"

int Server::_Accept(std::string &ip)
{
    struct sockaddr_in addr;
    socklen_t length = sizeof(addr);
    int fd = accept(m_lfd, (struct sockaddr *)&addr, &length);
    ip = inet_ntoa(addr.sin_addr);
    if (onConnect)
    {
        onConnect(fd, ip);
    }
    return fd;
}

int Server::_Receive(const int &fd, std::string &message)
{
    message.clear();
    message.resize(MESSAGE_MAX_LENGTH);
    int size = recv(fd, &message[0], message.size(), 0);
    if (size == 0)
    {
        if (onClose)
        {
            onClose(fd, message);
        }
        close(fd);
    }
    else if (size > 0)
    {
        if (onMessage)
        {
            onMessage(fd, message);
        }
    }
    else
    {
        std::perror("recv");
    }
    return size;
}

bool Server::_StartSelect()
{
    int max_fd = m_lfd;
    fd_set read_set;
    fd_set temp_set;
    FD_ZERO(&read_set);
    FD_SET(m_lfd, &read_set);
    std::string message;
    while (1)
    {
        temp_set = read_set;
        int num = select(max_fd + 1, &temp_set, NULL, NULL, NULL);
        if (FD_ISSET(m_lfd, &temp_set))
        {
            int client_fd = _Accept(message);
            FD_SET(client_fd, &read_set);
            if (client_fd > max_fd)
            {
                max_fd = client_fd;
            }
        }
        for (int fd = 0; fd <= max_fd; ++fd)
        {
            if (fd != m_lfd && FD_ISSET(fd, &temp_set))
            {
                int size = _Receive(fd, message);
                if (size == 0)
                {
                    FD_CLR(fd, &read_set);
                }
            }
        }
    }
    return true;
}

bool Server::_StartPoll()
{

    struct pollfd fds[FD_LIST_SIZE];
    for (int i = 0; i < FD_LIST_SIZE; ++i)
    {
        fds[i].fd = -1;
        fds[i].events = POLLIN;
    }
    fds[0].fd = m_lfd;
    int max_index = 0;
    std::string message;
    while (1)
    {
        int num = poll(fds, max_index + 1, -1);
        if (fds[0].revents & POLLIN)
        {
            int client_fd = _Accept(message);
            for (int i = 0; i < FD_LIST_SIZE; ++i)
            {
                if (fds[i].fd == -1)
                {
                    fds[i].fd = client_fd;
                    if (i > max_index)
                    {
                        max_index = i;
                    }
                    break;
                }
            }
        }
        for (int i = 1; i <= max_index; ++i)
        {
            if (fds[i].revents & POLLIN)
            {
                int size = _Receive(fds[i].fd, message);
                if (size == 0)
                {
                    fds[i].fd = -1;
                }
            }
        }
    }
    return true;
}

bool Server::_StartEpoll()
{
    int epoll_fd = epoll_create(1);
    if (epoll_fd < 0)
    {
        std::perror("epoll_create");
        return false;
    }
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = m_lfd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, m_lfd, &ev) < 0)
    {
        std::perror("epoll_ctl");
        return false;
    }
    struct epoll_event evs[FD_LIST_SIZE];
    std::string message;
    while (1)
    {
        int num = epoll_wait(epoll_fd, evs, FD_LIST_SIZE, -1);
        for (int i = 0; i < num; ++i)
        {
            int fd = evs[i].data.fd;
            if (fd == m_lfd)
            {
                int client_fd = _Accept(message);
                ev.events = EPOLLIN;
                ev.data.fd = client_fd;
                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) < 0)
                {
                    perror("epoll_ctl");
                }
            }
            else
            {
                int size = _Receive(fd, message);
                if (size == 0)
                {
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
                }
            }
        }
    }

    return true;
}

const int Server::GetFD()
{
    return m_lfd;
}

const unsigned short Server::GetPort()
{
    return m_port;
}

bool Server::Init(const unsigned short port)
{
    if (m_lfd != -1)
        return false;
    if ((m_lfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        std::perror("socket");
        return false;
    }

    int opt = 1;
    if (setsockopt(m_lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        Close();
        std::perror("setsockopt");
        return false;
    }

    m_port = port;
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(m_port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(m_lfd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        Close();
        std::perror("bind");
        return false;
    }
    if (listen(m_lfd, 10) < 0)
    {
        Close();
        std::perror("listen");
        return false;
    }
    return true;
}

bool Server::Bind(const EventType &event, CallbackType function)
{
    switch (event)
    {
    case EventType::OnConnect:
        onConnect = function;
        return true;
    case EventType::OnMessage:
        onMessage = function;
        return true;
    case EventType::OnClose:
        onClose = function;
        return true;
    default:
        return false;
    }
}

bool Server::Start(const IOType &type)
{
    switch (type)
    {
    case IOType::Select:
        return _StartSelect();
    case IOType::Poll:
        return _StartPoll();
    case IOType::Epoll:
        return _StartEpoll();
    default:
        return false;
    }
}

bool Server::Send(const int &fd, const std::string &message)
{
    if (fd == -1)
        return false;
    if (send(fd, message.data(), message.size(), 0) <= 0)
        return false;
    return true;
}

bool Server::Close()
{
    if (m_lfd == -1)
        return false;
    close(m_lfd);
    m_lfd = -1;
    return true;
}
