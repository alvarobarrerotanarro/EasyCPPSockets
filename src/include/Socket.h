#pragma once

#include <iostream>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstdlib>
#include <cstdint>
#include <string>
#include <vector>
#include <stdexcept>
#include <memory>
#include <cstring>
#include <cctype>
#include <sstream>
#include <cmath>
#include <chrono>

#include "SocketDescriptor.h"

class ServerSocket;
class SocketBuffer;

class Socket
{
    friend class ServerSocket;
    friend class SocketBuffer;

private:
    SocketDescriptor descriptor;
    std::unique_ptr<SocketBuffer> sockStreamBuffer;
    std::unique_ptr<std::iostream> sockStream;

    /*
     * Server to client socket.
     */
    explicit Socket(SocketDescriptor &&descriptor);

public:
    /*
     * Client to server socket.
     */
    Socket(std::string serverIp, std::uint16_t serverPort);

    Socket(const Socket &other) = delete;
    Socket(Socket &&other) noexcept;

    Socket &operator=(const Socket &other) = delete;
    Socket &operator=(Socket &&other) noexcept;

    std::iostream &getSockStream()
    {
        return *sockStream.get();
    }

    template <typename Rep, typename Period>
    void setOsTimeout(const std::chrono::duration<Rep, Period> &duration)
    {
        timeval tv;
        tv.tv_usec = 0;
        tv.tv_sec = std::chrono::duration_cast<std::chrono::seconds>(duration).count();

        if (setsockopt(descriptor, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0)
        {
            throw std::runtime_error{std::string{"setsockopt failed: "} + strerror(errno)};
        }
    }

    /**
     * There is no guarantee the entire buffer will be full as it depends on whatever is available at that moment.
     */
    inline ssize_t recv(void *buffer, size_t bufferBytesLength)
    {
        return ::recv(descriptor, buffer, bufferBytesLength, 0);
    }

    /**
     * There is no guarantee the entire buffer will be send as it depends on whatever the os decides.
     */
    inline ssize_t send(void *buffer, size_t bufferBytesLength)
    {
        return ::send(descriptor, buffer, bufferBytesLength, 0);
    }

    /**
     * Reads bloquing the current thread until the kernel supplies the
     * requested ammount of bytes retuning the actual ammount of
     * bytes read (the peer closed the connection and it was not possible
     * to send all the data) or -1 in case of error.
     */
    inline ssize_t recvAll(void *buffer, size_t bufferBytesLength)
    {
        bool failure = false;
        bool peerIsClosed = false;
        size_t totalBytesRead = 0;
        ssize_t bytesRead = -1;

        do
        {
            bytesRead = ::recv(descriptor, static_cast<char *>(buffer) + totalBytesRead, bufferBytesLength - totalBytesRead, 0);

            if (bytesRead == -1)
            {
                failure = true;
            }
            else if (bytesRead == 0)
            {
                peerIsClosed = true;
            }
            else
            {
                totalBytesRead += bytesRead;
            }

        } while (totalBytesRead < bufferBytesLength && !peerIsClosed && !failure);

        return !failure ? totalBytesRead : -1;
    }

    /**
     * Sends bloquing the current thread until the kernel dispatches the
     * specified ammount of bytes or -1 in case of error.
     */
    inline ssize_t sendAll(void *buffer, size_t bufferBytesLength)
    {
        bool failure = false;
        size_t totalBytesWritten = 0;
        ssize_t bytesWritten = -1;

        do
        {
            bytesWritten = ::send(descriptor, static_cast<char *>(buffer) + totalBytesWritten, bufferBytesLength - totalBytesWritten, 0);

            if (bytesWritten == -1)
            {
                failure = true;
            }
            else
            {
                totalBytesWritten += bytesWritten;
            }

        } while (totalBytesWritten < bufferBytesLength && !failure);

        return !failure ? bytesWritten : -1;
    }

    virtual ~Socket();
};