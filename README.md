# Socket Programming

A chat server and client by c++ socket programming.

## Build

```bash
$ cmake .
$ make
```

## Run

### Server

```bash
$ ./bin/server 5000
server on: 5000
client connected: 127.0.0.1
received from client: hello server
send to client: hello client
```

### Client

```bash
$ ./bin/client localhost 5000
connected
send to server: hello server
receive from server: hello client
send to server: done
```
