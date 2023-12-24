
#include "../server/include/server.h"

int Server::start() {
  WSADATA wsaData;
  if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
    std::cerr << "Failed to initialize WinSock." << std::endl;
    return 1;
  }

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == INVALID_SOCKET) {
    std::cerr << "Failed to set socket file descriptor. Error: " << WSAGetLastError() << std::endl;
    return 1;
  }

  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const char*) &opt, sizeof(opt)) == SOCKET_ERROR) {
    std::cerr << "Failed to set socket options. Error: " << WSAGetLastError() << std::endl;
    return 1;
  }

  address.sin_family = AF_INET;
  address.sin_port = htons(PORT);
  address.sin_addr.s_addr = INADDR_ANY;

  listenConnections();
  
  WSACleanup();
  return 0;
}

int Server::listenConnections() {
  if (bind(sockfd, (const sockaddr*) &address, sizeof(address)) < 0) {
    std::cerr << "Failed to bind socket. Error: " << WSAGetLastError() << std::endl;
    return 1;
  }

  if (listen(sockfd, MAXIMUM_PENDING_CONNECTIONS) < 0) {
    std::cerr << "Failed to listen." << std::endl;
    return 1;
  } else {
    std::cout << "Server is listening on PORT: " << PORT << std::endl;
  }

  while (TRUE) {
    int clientSocket = accept(sockfd, (struct sockaddr*) &address, &addrlen);
    if (clientSocket < 0) {
      std::cerr << "Failed to accept connection." << std::endl;
      return 1;
    }

    workerThreads.emplace_back(&Server::handleConnections, this, clientSocket);

    if (workerThreads.size() >= MAXIMUM_CONCURRENT_CONNECTIONS) {
      workerThreads.front().join();
      workerThreads.erase(workerThreads.begin());
    }
  }

  return 0;
}

int Server::handleConnections(int clientSocket) {
  std::cout << "Connection received!" << std::endl;

  const char* response = "HTTP/1.1 200 OK\r\nContent-Length: 12\r\n\r\nHello World!";
  if (send(clientSocket, response, strlen(response), 0) == SOCKET_ERROR) {
    std::cerr << "Failed to send response. Error: " << WSAGetLastError() << std::endl;
  }

  closesocket(clientSocket);

  return 0;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: ./server <PORT>" << std::endl;
    return 1;
  }

  int serverPORT = std::stoi(argv[1]);

  std::cout << "Starting server on PORT: " << serverPORT << std::endl;

  Server server(serverPORT);

  server.start();

  return 0;
}