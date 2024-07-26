#ifndef SERVER_H_
#define SERVER_H_
#include <iostream>
#include <functional>

using CallbackType = std::function<void(const int &, std::string &)>;

enum EventType
{
    OnConnect,
    OnMessage,
    OnClose,
};

enum IOType
{
    Select,
    Poll,
    Epoll
};

class Server
{
private:
    int m_lfd;
    unsigned short m_port;
    CallbackType onConnect;
    CallbackType onMessage;
    CallbackType onClose;

    int _Accept(std::string &ip);
    int _Receive(const int &fd, std::string &message);

    bool _StartSelect();
    bool _StartPoll();
    bool _StartEpoll();

public:
    Server() : m_lfd(-1), onConnect(nullptr), onMessage(nullptr), onClose(nullptr) {};
    bool Init(const unsigned short port);
    bool Bind(const EventType &event, CallbackType);
    bool Start(const IOType &type);
    bool Send(const int &fd, const std::string &message);
    bool Close();
    ~Server() { Close(); };
};

#endif