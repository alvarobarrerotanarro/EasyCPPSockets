#include <iostream>
#include <string>
#include <cstdint>
#include <memory>
#include <chrono>

#include "EasyCPPSockets/SocketDescriptor.h"
#include "EasyCPPSockets/Socket.h"
#include "EasyCPPSockets/SocketBuffer.h"

using namespace easycppsockets;

Socket::Socket(SocketDescriptor &&descriptor)
    : descriptor{std::move(descriptor)}
{
    setOsTimeout(std::chrono::seconds{5});
    sockStreamBuffer = SocketBuffer::make(*this);
    sockStream = std::make_unique<std::iostream>(sockStreamBuffer.get());
}

Socket::Socket(const std::string &serverIp, std::uint16_t serverPort)
    : descriptor{-1}
{
    sockStreamBuffer = SocketBuffer::make(*this);
    sockStream = std::make_unique<std::iostream>(sockStreamBuffer.get());

    // Open socket FD
    descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (descriptor == -1)
    {
        throw std::runtime_error{"Client socket open failed"};
    }
    setOsTimeout(std::chrono::seconds{5});

    // Configure server address
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(serverPort);
    if (inet_pton(AF_INET, serverIp.c_str(), &serverAddress.sin_addr) != 1)
    {
        throw std::runtime_error{"Invalid server socket addr"};
    }

    // Begin the connection
    int connectResult = -1;
    do
    {
        connectResult = connect(descriptor, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    } while (connectResult == -1 && errno == EINTR);

    if (connectResult == -1 && errno != EINTR)
    {
        throw std::runtime_error{"Connect failed"};
    }
}

Socket::Socket(Socket &&other) noexcept
    : sockStreamBuffer{std::move(other.sockStreamBuffer)}, sockStream(std::move(other.sockStream)), descriptor{std::move(other.descriptor)}
{
    if (this->sockStreamBuffer != nullptr)
    {
        this->sockStreamBuffer->socket_ = this;
    }
}

Socket &Socket::operator=(Socket &&other) noexcept
{
    if (this != &other)
    {
        this->descriptor = std::move(other.descriptor);
        this->sockStreamBuffer = std::move(other.sockStreamBuffer);
        this->sockStream = std::move(other.sockStream);

        if (this->sockStreamBuffer != nullptr)
        {
            this->sockStreamBuffer->socket_ = this;
        }
    }

    return *this;
}

Socket::~Socket() = default;