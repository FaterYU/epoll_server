#ifndef EPOLL__EPOLL_HPP
#define EPOLL__EPOLL_HPP

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <atomic>
#include <cstring>
#include <iostream>
#include <memory>

#define MAX_EVENTS 10
namespace epoll {

class Epoll {
 public:
  explicit Epoll(const int max_events, const int port, const int buffer_size);
  ~Epoll();

  void spin();

 private:
  void init();

  int port_;
  int max_events_;
  int buffer_size_;

  int epoll_fd_;
  int socket_fd_;
  struct epoll_event socket_event_;
  std::unique_ptr<char[]> buffer_;
  std::unique_ptr<struct epoll_event[]> events_;
};
};  // namespace epoll

#endif  // EPOOLL__EPOLL_HPP
