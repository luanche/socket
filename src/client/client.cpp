#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

class Client
{
private:
  int m_client_fd;
  std::string m_ip;
  unsigned short m_port;

public:
  Client() : m_client_fd(-1) {};
  bool Connect(const std::string &ip, const unsigned short port);
  ~Client() {};
};

bool Client::Connect(const std::string &ip, const unsigned short port)
{
  if ((m_client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    std::perror("socket");
    return false;
  }
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);

  struct hostent *host;
  m_ip = ip;
  if ((host = gethostbyname(m_ip.c_str())) == nullptr)
  {
    std::perror("gethostbyname");
    return false;
  }

  memcpy(&addr.sin_addr, host->h_addr_list[0], host->h_length);
}
