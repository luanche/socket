#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "server.h"

#define MESSAGE_MAX_LENGTH 1024

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
            struct sockaddr_in addr;
            socklen_t length = sizeof(addr);
            int client_fd = accept(m_lfd, (struct sockaddr *)&addr, &length);
            FD_SET(client_fd, &read_set);
            message = inet_ntoa(addr.sin_addr);
            if (client_fd > max_fd)
            {
                max_fd = client_fd;
            }
            if (onConnect)
            {
                onConnect(client_fd, message);
            }
        }
        for (int fd = 0; fd <= max_fd; fd++)
        {
            if (fd != m_lfd && FD_ISSET(fd, &temp_set))
            {
                message.clear();
                message.resize(MESSAGE_MAX_LENGTH);
                int size = recv(fd, &message[0], message.size(), 0);
                if (size == 0)
                {
                    FD_CLR(fd, &read_set);
                    close(fd);
                    if (onClose)
                    {
                        onClose(fd, message);
                    }
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
                    perror("recv");
                }
            }
        }
    }
}

bool Server::Init(const unsigned short port)
{
    if (m_lfd != -1)
        return false;
    if ((m_lfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        return false;
    }

    int opt = 1;
    if (setsockopt(m_lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        Close();
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
        return false;
    }
    if (listen(m_lfd, 10) < 0)
    {
        Close();
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
        _StartSelect();
        return true;
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
