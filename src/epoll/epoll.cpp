#include "epoll/epoll.hpp"

namespace epoll {

Epoll::Epoll(const int max_events, const int port, const int buffer_size)
    : max_events_(max_events), port_(port), buffer_size_(buffer_size) {
  init();
  std::cout << "Epoll initialized" << std::endl;
}

Epoll::~Epoll() {
  close(socket_fd_);
  close(epoll_fd_);
  std::cout << "Epoll destroyed" << std::endl;
}

void Epoll::spin() {
  int recored_max_events = 0;
  while (true) {
    int n = epoll_wait(epoll_fd_, events_.get(), max_events_, -1);
    if (n == -1) {
      perror("epoll_wait");
      exit(EXIT_FAILURE);
    }
    if (n > recored_max_events) {
      recored_max_events = n;
      std::cout << "Max events: " << recored_max_events << std::endl;
    }
    for (int i = 0; i < n; i++) {
      if (events_[i].data.fd == socket_fd_) {
        // new connection
        sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int client_fd = accept(socket_fd_, (struct sockaddr *)&client_addr,
                               &client_addr_len);
        if (client_fd == -1) {
          perror("accept");
          exit(EXIT_FAILURE);
        }

        // set client socket to non-blocking
        int flags = fcntl(client_fd, F_GETFL, 0);
        if (flags == -1) {
          perror("fcntl");
          exit(EXIT_FAILURE);
        }
        flags |= O_NONBLOCK;
        if (fcntl(client_fd, F_SETFL, flags) == -1) {
          perror("fcntl");
          exit(EXIT_FAILURE);
        }

        // add client socket to epoll
        struct epoll_event client_event;
        client_event.events = EPOLLIN | EPOLLET;
        client_event.data.fd = client_fd;
        if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, client_fd, &client_event) ==
            -1) {
          perror("epoll_ctl");
          exit(EXIT_FAILURE);
        }
      } else {
        if (events_[i].events & EPOLLERR || events_[i].events & EPOLLHUP) {
          epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, events_[i].data.fd, NULL);
          close(events_[i].data.fd);
        } else if (events_[i].events & EPOLLIN) {
          // read data
          int n = read(events_[i].data.fd, buffer_.get(), buffer_size_);
          if (n == -1) {
            perror("read");
            exit(EXIT_FAILURE);
          }

          // echo data
          n = write(events_[i].data.fd, buffer_.get(), n);
          if (n == -1) {
            perror("write");
            exit(EXIT_FAILURE);
          }
        }
      }
    }
  }
}

void Epoll::init() {
  // set parameters
  buffer_ = std::make_unique<char[]>(buffer_size_);
  events_ = std::make_unique<struct epoll_event[]>(max_events_);

  // set TCP socket
  socket_fd_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (socket_fd_ == -1) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port_);
  addr.sin_addr.s_addr = INADDR_ANY;
  if (bind(socket_fd_, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
    perror("bind");
    exit(EXIT_FAILURE);
  }

  //   start listening
  if (listen(socket_fd_, SOMAXCONN) == -1) {
    perror("listen");
    exit(EXIT_FAILURE);
  }

  //  create epoll instance
  epoll_fd_ = epoll_create(1);
  if (epoll_fd_ == -1) {
    perror("epoll_create");
    exit(EXIT_FAILURE);
  }

  //   add socket to epoll
  socket_event_.events = EPOLLIN;
  socket_event_.data.fd = socket_fd_;
  if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, socket_fd_, &socket_event_) == -1) {
    perror("epoll_ctl");
    exit(EXIT_FAILURE);
  }
}

}  // namespace epoll
