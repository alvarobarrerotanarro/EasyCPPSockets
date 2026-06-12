#pragma once

#include <arpa/inet.h>
#include <sys/socket.h>
#include <cstdint>
#include <string>
#include <tuple>
#include <stdexcept>
#include <algorithm>
#include <cstring>

#include "EasyCPPSockets/SocketDescriptor.h"

namespace easycppsockets
{
    class Socket;
    class ServerSocket;

    class SocketAddress
    {
        friend class Socket;
        friend class ServerSocket;

    private:
        std::uint16_t port;
        std::string presentationAddress;

        SocketAddress(const std::string presentationAddress, std::uint16_t port)
            : presentationAddress{presentationAddress}, port{port}
        {
        }

        SocketAddress()
            : presentationAddress{""}, port{0}
        {
        }

        static SocketAddress make(SocketDescriptor &descriptor)
        {
            std::uint8_t addrBuffer[28] = {0};
            sockaddr *addr = reinterpret_cast<sockaddr *>(addrBuffer);
            socklen_t addrLen = 28;

            if (getsockname(descriptor, addr, &addrLen) == -1)
            {
                throw std::runtime_error{std::string{"getsockname failed: "} + strerror(errno)};
            }

            std::string presentationAddress = getPresentationAddress(addr, addrLen);
            std::uint16_t port = getPort(addr, addrLen);

            return {presentationAddress, port};
        }

        static std::string getPresentationAddress(struct sockaddr *addr, socklen_t addrLen)
        {
            constexpr int presAddrBuffSize = std::max(INET_ADDRSTRLEN, INET6_ADDRSTRLEN);
            char presAddrBuff[presAddrBuffSize] = {0};

            if (addrLen == sizeof(sockaddr_in))
            {
                inet_ntop(
                    AF_INET,
                    &reinterpret_cast<sockaddr_in *>(addr)->sin_addr,
                    presAddrBuff,
                    presAddrBuffSize
                );
            }
            else if (addrLen == sizeof(sockaddr_in6))
            {
                inet_ntop(
                    AF_INET6,
                    &reinterpret_cast<sockaddr_in6 *>(addr)->sin6_addr,
                    presAddrBuff,
                    presAddrBuffSize
                );
            }

            char *presAddrBuffEnd = std::find(std::begin(presAddrBuff), std::end(presAddrBuff), '\0');
            return {presAddrBuff, presAddrBuffEnd};
        }

        static std::uint16_t getPort(struct sockaddr *addr, socklen_t addrLen)
        {
            if (addrLen == sizeof(sockaddr_in))
                return ntohs(((sockaddr_in *)addr)->sin_port);
            if (addrLen == sizeof(sockaddr_in6))
                return ntohs(((sockaddr_in6 *)addr)->sin6_port);
            return 0;
        }

    public:
        std::string getPresentationAddress()
        {
            return presentationAddress;
        }

        std::uint16_t getPort()
        {
            return port;
        }
    };
}