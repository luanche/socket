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

std::string &Server::GetClientIP()
{
    return m_client_ip;
}

bool Server::Init(const unsigned short port)
{
    if (m_server_fd != -1)
        return false;
    if ((m_server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        std::perror("socket");
        return false;
    }
    m_port = port;
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(m_port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(m_server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        CloseServer();
        return false;
    }
    if (listen(m_server_fd, 10) < 0)
    {
        CloseServer();
        return false;
    }
    return true;
}

bool Server::Accept()
{
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    if ((m_client_fd = accept(m_server_fd, (struct sockaddr *)&addr, &addrlen)) < 0)
    {
        return false;
    }
    m_client_ip = inet_ntoa(addr.sin_addr);
    return true;
}

bool Server::Send(const std::string &message)
{
    if (m_client_fd == -1)
        return false;
    if (send(m_client_fd, message.data(), message.size(), 0) <= 0)
        return false;
    return true;
}

bool Server::Receive(std::string &message, size_t max_length)
{
    if (m_client_fd == -1)
    {
        return false;
    }
    message.clear();
    message.resize(max_length);

    int size = recv(m_client_fd, &message[0], message.size(), 0);
    if (size <= 0)
    {
        message.clear();
        return false;
    }
    message.resize(size);
    return true;
}

void Server::Close()
{
    CloseServer();
    CloseClient();
}

bool Server::CloseServer()
{
    if (m_server_fd == -1)
        return false;
    close(m_server_fd);
    m_server_fd = -1;
    return true;
}

bool Server::CloseClient()
{
    if (m_client_fd == -1)
        return false;
    close(m_client_fd);
    m_client_fd = -1;
    return true;
}