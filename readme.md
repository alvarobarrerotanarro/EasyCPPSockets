# EasyCPPSockets

![Build](https://github.com/alvarobarrerotanarro/EasyCPPSockets/actions/workflows/build.yml/badge.svg)
![Build](https://github.com/alvarobarrerotanarro/EasyCPPSockets/actions/workflows/test.yml/badge.svg)
![Build](https://github.com/alvarobarrerotanarro/EasyCPPSockets/actions/workflows/pingpong-example.yml/badge.svg)

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
* CMake
* GCC
* POSIX sockets

---

# Installation

## CMake Integration

EasyCPPSockets can be consumed in two different ways depending on your workflow:

* **FetchContent**: recommended when EasyCPPSockets is built as part of your project.
* **find_package**: recommended when EasyCPPSockets has already been installed on the system or in a custom installation prefix.

---

## Using FetchContent

`FetchContent` downloads and builds EasyCPPSockets as part of your project's build process.

```cmake
cmake_minimum_required(VERSION 3.14)

project(MyApplication)

include(FetchContent)

FetchContent_Declare(
    EasyCPPSockets
    GIT_REPOSITORY https://github.com/alvarobarrerotanarro/EasyCPPSockets.git
    GIT_TAG main
)

FetchContent_MakeAvailable(EasyCPPSockets)

add_executable(MyApplication
    src/main.cpp
)

target_link_libraries(MyApplication
    PRIVATE EasyCPPSockets::EasyCPPSockets
)
```

This approach is ideal when:

* You want fully reproducible builds.
* You do not want users to install dependencies manually.
* EasyCPPSockets is only used by a single project.

---

## Using find_package

EasyCPPSockets can also be installed and discovered through CMake's `find_package()` mechanism.

First install the library:

```bash
./configure-release && ./build && ./install
```

If you want to install into a custom location:

```bash
cmake -B cmake-build -DCMAKE_INSTALL_PREFIX=/path/to/install && ./build && ./install
```

Then consume the package from another project:

```cmake
cmake_minimum_required(VERSION 3.14)

project(MyApplication)

find_package(EasyCPPSockets REQUIRED)

add_executable(MyApplication
    src/main.cpp
)

target_link_libraries(MyApplication
    PRIVATE EasyCPPSockets::EasyCPPSockets
)
```

If EasyCPPSockets was installed into a custom directory, tell CMake where to search:

```bash
cmake -B cmake-build \
    -DCMAKE_PREFIX_PATH=/path/to/install
```

Alternatively, you can point directly to the package configuration directory:

```bash
cmake -B cmake-build \
    -DEasyCPPSockets_DIR=/path/to/install/lib/cmake/EasyCPPSockets
```

This approach is recommended when:

* EasyCPPSockets is shared between multiple projects.
* The library is managed by a package manager.
* You want faster configuration times by avoiding dependency downloads.

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

