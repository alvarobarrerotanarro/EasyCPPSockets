#pragma once

#include <stdexcept>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstdint>
#include <memory>

#include "Socket.h"

class ServerSocket
{
private:
    SocketDescriptor descriptor;

public:
    ServerSocket(std::uint16_t port, int maxSimultanealConnections)
        : descriptor{-1}
    {
        descriptor = socket(AF_INET, SOCK_STREAM, 0);
        if (descriptor == -1)
        {
            throw std::runtime_error{"Server socket open failed"};
        }

        sockaddr_in serverAddress;
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(port);
        serverAddress.sin_addr.s_addr = INADDR_ANY;

        if (bind(descriptor, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
        {
            throw std::runtime_error{std::string{"Bind failed: "} + strerror(errno)};
        }

        if (listen(descriptor, maxSimultanealConnections) == -1)
        {
            throw std::runtime_error{"Server socket listen failed"};
        }
    }

    ServerSocket(const ServerSocket &other) = delete;
    ServerSocket(ServerSocket &&other)
        : descriptor{-1}
    {
        this->descriptor = std::move(other.descriptor);
    }

    ServerSocket &operator=(ServerSocket &&other)
    {
        if (this != &other)
        {
            this->descriptor = std::move(other.descriptor);
        }

        return *this;
    }

    ServerSocket &operator=(const ServerSocket &other) = delete;

    std::unique_ptr<Socket> accept()
    {
        SocketDescriptor clientDescriptor = ::accept(descriptor, nullptr, nullptr);
        if (clientDescriptor == -1)
        {
            throw std::runtime_error{"Client socket open failed"};
        }

        return std::unique_ptr<Socket>(new Socket{std::move(clientDescriptor)});
    }

    virtual ~ServerSocket()
    {
    }
};