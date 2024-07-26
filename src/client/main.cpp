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
    std::perror("connect");
    return -1;
  }
  std::cout << "connected to " << argv[1] << " " << argv[2] << std::endl;
  std::string message;
  while (1)
  {
    std::cout << "send to server: ";
    std::getline(std::cin, message);
    if (message == "")
    {
      break;
    }
    if (!client.Send(message))
    {
      std::perror("send");
      break;
    }
    if (!client.Receive(message, 1024))
    {
      std::perror("receive");
      break;
    }
    std::cout << "received from server: " << message << std::endl;
  }
  client.Close();
  return 0;
}