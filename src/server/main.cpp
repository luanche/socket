#include <iostream>
#include <algorithm>
#include <cctype>
#include "server.h"

#define USAGE "usage: ./bin/server port [select|poll|epoll]\nexample: ./bin/server 5000 epoll\n\n"

int main(int argc, char *argv[])
{
  if (argc < 2 || argc > 3)
  {
    std::cout << USAGE;
    return -1;
  }

  Server server;
  if (!server.Init(atoi(argv[1])))
  {
    return -1;
  }

  IOType type = IOType::Select;
  std::string type_name = "select";
  if (argc == 3)
  {
    type_name = argv[2];
    if (type_name == "select")
    {
      type = IOType::Select;
    }
    else if (type_name == "poll")
    {
      type = IOType::Poll;
    }
    else if (type_name == "epoll")
    {
      type = IOType::Epoll;
    }
    else
    {
      std::cerr << "wrong type\n\n";
      std::cout << USAGE;
      return -1;
    }
  }

  server.Bind(EventType::OnConnect, [](const int &fd, std::string &ip)
              { std::cout << "client connected " << fd << ": " << ip << std::endl; });

  server.Bind(EventType::OnClose, [](const int &fd, std::string &message)
              { std::cout << "client disconnected " << fd << ": " << message << std::endl; });

  server.Bind(EventType::OnMessage, [&server](const int &fd, std::string &message)
              { std::cout << "received from client " << fd << ": " << message << std::endl;
                std::transform(message.begin(), message.end(), message.begin(), toupper);
                server.Send(fd, message);
                std::cout << "send to client " << fd << ": " << message << std::endl; });

  std::cout << "server start"
            << ", fd: " << server.GetFD()
            << ", port: " << server.GetPort()
            << ", type: " << type_name
            << std::endl;

  server.Start(type);

  server.Close();
  return 0;
}