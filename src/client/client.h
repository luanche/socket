#ifndef CLIENT_H_
#define CLIENT_H_
#include <iostream>

class Client
{
private:
    int m_client_fd;
    std::string m_ip;
    unsigned short m_port;

public:
    Client() : m_client_fd(-1){};
    bool Connect(const std::string &ip, const unsigned short port);
    bool Send(const std::string &message);
    bool Receive(std::string &message, size_t max_length);
    bool Close();
    ~Client() { Close(); };
};

#endif