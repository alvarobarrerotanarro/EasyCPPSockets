#pragma once

#include <iostream>
#include <array>
#include <memory>

namespace easycppsockets
{
    class Socket;

    class SocketBuffer : public std::streambuf
    {
        friend class Socket;

    private:
        static constexpr size_t kBufferSize = 1024;

        Socket *socket_;
        std::array<char, kBufferSize> readArea_;
        std::array<char, kBufferSize> writeArea_;

        SocketBuffer(Socket *socket);

        static std::unique_ptr<SocketBuffer> make(Socket &socket)
        {
            return std::unique_ptr<SocketBuffer>(new SocketBuffer(&socket));
        }

    protected:
        int underflow() override;

        int overflow(int ch) override;

        inline int sync() override
        {
            return overflow(traits_type::eof()) != traits_type::eof() ? 0 : -1;
        }

    public:
        SocketBuffer(const SocketBuffer &other) = delete;
        SocketBuffer(SocketBuffer &&other) noexcept
            : socket_{other.socket_}
        {
            this->socket_ = other.socket_;
            this->readArea_ = std::move(other.readArea_);
            this->writeArea_ = std::move(other.writeArea_);
        }

        SocketBuffer &operator=(const SocketBuffer &other) = delete;

        SocketBuffer &operator=(SocketBuffer &&other) noexcept
        {
            if (this != &other)
            {
                this->socket_ = other.socket_;
                this->readArea_ = std::move(other.readArea_);
                this->writeArea_ = std::move(other.writeArea_);
            }

            return *this;
        }

        virtual ~SocketBuffer() = default;
    };
};