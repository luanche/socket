#include <iostream>
#include "client.h"

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    std::cout << "Using:./bin/client host port\nExample:./bin/client localhost 5000\n\n";
    return -1;
  }

  Client client;
  if (!client.Connect(argv[1], atoi(argv[2])))
  {
    std::perror("connect error");
    return -1;
  }
  std::cout << "connected\n";
  std::string message;
  while (1)
  {
    std::cout << "send to server: ";
    std::getline(std::cin, message);
    if (message.compare("done") == 0)
    {
      break;
    }
    if (!client.Send(message))
    {
      std::perror("send error");
      break;
    }
    if (!client.Receive(message, 1024))
    {
      std::perror("receive error");
      break;
    }
    std::cout << "receive from server: " << message << std::endl;
  }
  client.Close();
  return 0;
}