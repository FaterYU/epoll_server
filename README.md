# epoll_server

## file structure

```bash
.
├── CMakeLists.txt              # cmake file
├── README.md                   # readme file
├── bin
│   └── epoll_server_node       # epoll server binary file
├── include
│   └── epoll
│       └── epoll.hpp           # epoll server header file
├── lib
│   └── libepoll_server.so      # epoll server library file
├── src
│   ├── epoll
│   │   └── epoll.cpp           # epoll server source file
│   └── main.cpp                # main source file
└── test-client.py              # test client source file
```

## compile

```bash
cd epoll_server
mkdir build
cd build
cmake ..
make
cd ..
```

## config

- address: localhost
- port: 8080

## bringup

epoll server bringup

```bash
./bin/epoll_server_node
```

test client bringup

```bash
ulimit -HSn 65536
python3 test-client.py
```
