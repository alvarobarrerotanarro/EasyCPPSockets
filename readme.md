# EasyCPPSockets

A lightweight modern C++ TCP socket library designed to provide a simple and idiomatic interface for client-server communication.

The library wraps POSIX sockets behind RAII abstractions and integrates seamlessly with the C++ standard stream API.

## Features

* Modern C++ API
* RAII socket management
* TCP client and server support
* Blocking and full-transfer operations
* `std::iostream` integration
* Exception-based error handling
* Move semantics support
* Easy integration with CMake

## Requirements

* Linux
* C++17 or newer
* POSIX sockets

---

# Installation

## CMake Integration

The recommended way to consume EasyCPPSockets is through `ExternalProject`.

Create a file named:

```text
cmake-scripts/EasyCPPSocketsSetup.cmake
```

with the following content:

```cmake
include(ExternalProject)

set(EasyCPPSockets_INSTALL_DIR "${CMAKE_BINARY_DIR}/external/EasyCPPSockets")
set(EasyCPPSockets_INCLUDE_DIR "${EasyCPPSockets_INSTALL_DIR}/include")
set(EasyCPPSockets_LIB_DIR "${EasyCPPSockets_INSTALL_DIR}/lib")

file(MAKE_DIRECTORY "${EasyCPPSockets_INCLUDE_DIR}")
file(MAKE_DIRECTORY "${EasyCPPSockets_LIB_DIR}")

ExternalProject_Add(easy_cpp_sockets_setup
    GIT_REPOSITORY https://github.com/alvarobarrerotanarro/EasyCPPSockets.git
    GIT_TAG main
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EasyCPPSockets_INSTALL_DIR}
)

add_library(EasyCPPSockets SHARED IMPORTED GLOBAL)

set_target_properties(EasyCPPSockets PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${EasyCPPSockets_INCLUDE_DIR}"
    IMPORTED_LOCATION
    "${EasyCPPSockets_LIB_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}EasyCPPSockets${CMAKE_SHARED_LIBRARY_SUFFIX}"
)

add_dependencies(EasyCPPSockets easy_cpp_sockets_setup)
```

Then include the setup script from your project:

```cmake
cmake_minimum_required(VERSION 3.10)

project(MyApplication)

include(${CMAKE_SOURCE_DIR}/cmake-scripts/EasyCPPSocketsSetup.cmake)

add_executable(MyApplication
    src/main.cpp
)

target_link_libraries(MyApplication
    PRIVATE EasyCPPSockets
)
```

---

# Quick Start

## TCP Server

```cpp
#include <EasyCPPSockets/ServerSocket.h>

int main()
{
    easycppsockets::ServerSocket server(3000, 10);

    auto client = server.accept();

    return 0;
}
```

The server:

* Listens on port `3000`
* Accepts up to `10` pending connections
* Blocks until a client connects

---

## TCP Client

```cpp
#include <EasyCPPSockets/Socket.h>

int main()
{
    easycppsockets::Socket socket(
        "127.0.0.1",
        3000
    );

    return 0;
}
```

The constructor immediately attempts to establish the TCP connection.

If the connection fails a `std::runtime_error` exception is thrown.

---

# Data Transmission

## send()

Sends up to the requested number of bytes.

```cpp
char message[] = "Hello";

ssize_t sent = socket.send(
    message,
    sizeof(message)
);
```

### Important

The operating system is not required to send the entire buffer in a single call.

Always check the returned value.

---

## recv()

Receives up to the requested number of bytes.

```cpp
char buffer[1024];

ssize_t received = socket.recv(
    buffer,
    sizeof(buffer)
);
```

### Important

The operating system may return fewer bytes than requested.

Always inspect the returned value.

---

# Guaranteed Transfer Operations

For most application protocols you will usually want to use `sendAll()` and `recvAll()`.

---

## sendAll()

Blocks until the complete buffer has been transmitted.

```cpp
std::string msg = "Hello";

socket.sendAll(
    msg.data(),
    msg.size()
);
```

### Behavior

* Retries partial sends automatically
* Throws `std::runtime_error` on failure

---

## recvAll()

Blocks until:

* The requested number of bytes has been received
* Or the peer closes the connection

```cpp
char buffer[256];

std::size_t bytesRead =
    socket.recvAll(
        buffer,
        sizeof(buffer)
    );
```

### Return Value

Returns the actual amount of bytes read.

If the returned value is smaller than the requested size, the remote peer closed the connection before all data could be received.

---

# Stream API

EasyCPPSockets provides direct integration with the C++ stream ecosystem.

```cpp
auto& stream = socket.getSockStream();

stream << "Hello server" << std::endl;

std::string response;
std::getline(stream, response);
```

This allows sockets to be used with:

* `std::getline`
* Stream operators (`<<`, `>>`)
* Existing stream-based utilities
* Custom serialization code

---

# Connection Timeouts

A receive timeout can be configured through `setOsTimeout()`.

```cpp
using namespace std::chrono_literals;

socket.setOsTimeout(5s);
```

### Behavior

If a receive operation exceeds the configured timeout:

```cpp
recv()
recvAll()
```

an exception will be thrown.

---

# Exception Handling

All networking errors are reported through `std::runtime_error`.

Example:

```cpp
try
{
    easycppsockets::Socket socket(
        "127.0.0.1",
        3000
    );
}
catch(const std::runtime_error& e)
{
    std::cerr << e.what() << '\n';
}
```

---

# Ownership Model

Socket objects are:

* Non-copyable
* Movable

```cpp
easycppsockets::Socket a(
    "127.0.0.1",
    3000
);

easycppsockets::Socket b =
    std::move(a);
```

The same rule applies to `ServerSocket`.

---

# Thread Safety

Individual socket instances should not be accessed concurrently from multiple threads unless external synchronization is provided by the application.

---

# Example Echo Server

```cpp
#include <EasyCPPSockets/ServerSocket.h>
#include <EasyCPPSockets/Socket.h>

int main()
{
    easycppsockets::ServerSocket server(3000, 10);

    auto client = server.accept();

    char buffer[1024];

    while(true)
    {
        ssize_t bytes =
            client->recv(
                buffer,
                sizeof(buffer)
            );

        if(bytes == 0)
        {
            break;
        }

        client->sendAll(
            buffer,
            bytes
        );
    }
}
```

