#include <iostream>
#include <algorithm>
#include <cctype>
#include "server.h"

int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    std::cout << "Using:./bin/server port\nExample:./bin/server 5000\n\n";
    return -1;
  }
  Server server;
  if (!server.Init(atoi(argv[1])))
  {
    std::perror("init error");
    return -1;
  }

  server.Bind(EventType::OnConnect, [](const int &fd, std::string &ip)
              { std::cout << "client connected " << fd << ": " << ip << std::endl; });

  server.Bind(EventType::OnClose, [](const int &fd, std::string &message)
              { std::cout << "client disconnected " << fd << ": " << message << std::endl; });

  server.Bind(EventType::OnMessage, [&server](const int &fd, std::string &message)
              {
                std::cout << "received from client " << fd << ": " << message << std::endl;
                std::transform(message.begin(), message.end(), message.begin(), toupper);
                server.Send(fd, message);
                std::cout << "send to client " << fd << ": " << message << std::endl; });

  server.Start(IOType::Epoll);

  server.Close();
  return 0;
}