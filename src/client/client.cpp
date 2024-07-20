#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "client.h"

bool Client::Connect(const std::string &ip, const unsigned short port)
{
  if (m_client_fd != -1)
    return false;
  if ((m_client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    std::perror("socket");
    return false;
  }
  m_ip = ip;
  m_port = port;
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(m_port);

  struct hostent *host;
  if ((host = gethostbyname(m_ip.c_str())) == nullptr)
  {
    std::perror("gethostbyname");
    Close();
    return false;
  }

  memcpy(&addr.sin_addr, host->h_addr, host->h_length);

  if (connect(m_client_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
  {
    std::perror("connect");
    Close();
    return false;
  }
  return true;
}

bool Client::Send(const std::string &message)
{
  if (m_client_fd == -1)
    return false;
  if (send(m_client_fd, message.data(), message.size(), 0) <= 0)
  {
    std::perror("send");
    return false;
  }
  return true;
}

bool Client::Receive(std::string &message, size_t max_length)
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

bool Client::Close()
{
  if (m_client_fd == -1)
    return false;
  close(m_client_fd);
  m_client_fd = -1;
  return true;
}