#ifndef SOCKET_H
#define SOCKET_H

#include <WinSock2.h>

#include <string>

enum TypeSocket { BlockingSocket, NonBlockingSocket };

class Socket {
 public:
  virtual ~Socket();
  Socket(const Socket&);
  Socket& operator=(Socket&);

  std::string ReceiveLine();
  std::string ReceiveBytes();

  void Close();

  void SendLine(std::string);
  void SendBytes(const std::string&);

 protected:
  friend class SocketServer;
  friend class SocketSelect;

  Socket(SOCKET s);
  Socket();

  SOCKET _socket;

  int* _refCounter;

 private:
  static void Start();
  static void End();
  static int _openSocketNum;
};

class SocketClient : public Socket {
 public:
  SocketClient(const std::string& host, int port);
};

class SocketServer : public Socket {
 public:
  SocketServer(int port, int connections, TypeSocket type = BlockingSocket);

  Socket* Accept();
};

class SocketSelect {
 public:
  SocketSelect(Socket const* const s1, Socket const* const s2 = NULL,
               TypeSocket type = BlockingSocket);

  bool Readable(Socket const* const s);

 private:
  fd_set fds_;
};

#endif