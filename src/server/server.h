#ifndef SERVER_H_
#define SERVER_H_
#include <iostream>

class Server
{
private:
    int m_server_fd;
    int m_client_fd;
    std::string m_client_ip;
    unsigned short m_port;

public:
    Server() : m_server_fd(-1), m_client_fd(-1){};
    std::string &GetClientIP();

    bool Init(const unsigned short port);
    bool Accept();
    bool Send(const std::string &message);
    bool Receive(std::string &message, size_t max_length);

    void Close();
    bool CloseServer();
    bool CloseClient();
    ~Server() { Close(); };
};

#endif