#include <iostream>
#include <algorithm>
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
  std::cout << "server on: " << argv[1] << std::endl;
  if (!server.Accept())
  {
    std::perror("accept error");
    return -1;
  }
  std::cout << "client connected: " << server.GetClientIP() << std::endl;
  std::string message;
  while (1)
  {
    if (!server.Receive(message, 1024))
    {
      std::perror("receive error");
      break;
    }
    std::cout << "received from client: " << message << std::endl;

    if (!server.Send("Client say:" + message))
    {
      std::perror("send error");
      break;
    }
  }
  server.Close();
  return 0;
}