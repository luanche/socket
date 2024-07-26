# Socket Programming

A chat server and client by c++ socket programming with select/poll/epoll

## Build

```bash
cmake . && make
```

## Run

### Server

#### Usage

```bash
./bin/server port [select|poll|epoll]
```

#### Example

```bash
$ ./bin/server 5000 epoll
server start, fd: 3, port: 5000, type: epoll
received from client 5: hello
send to client 5: HELLO
received from client 5: world
send to client 5: WORLD
client disconnected 5:
```

### Client

```bash
$ ./bin/client localhost 5000
connected to localhost 5000
send to server: hello server
received from server: HELLO SERVER
send to server:
```
