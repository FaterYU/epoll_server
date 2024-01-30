#include <epoll/epoll.hpp>

int main() {
  signal(SIGINT, [](int) { exit(EXIT_SUCCESS); });
  int max_events = 1000;
  int port = 8080;
  int buffer_size = 1024;
  epoll::Epoll epoll(max_events, port, buffer_size);
  epoll.spin();
  return 0;
}
