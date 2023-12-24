#ifndef SERVER_H
#define SERVER_H

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <vector>
#include <thread>

#define MAXIMUM_CONCURRENT_CONNECTIONS 1000
#define MAXIMUM_PENDING_CONNECTIONS 10

class Server {
  int sockfd;
  struct sockaddr_in address;
  socklen_t addrlen = sizeof(address);
  const int opt = 1;
  std::vector<std::thread> workerThreads;
  
  int PORT;

  int handleConnections(int clientSocket);

public:
  Server(int PORT) : PORT(PORT){};

  int start();
  int listenConnections();
};

#endif