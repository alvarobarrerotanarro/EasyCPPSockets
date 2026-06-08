#include <streambuf>
#include <memory>

#include "Socket.h"
#include "SocketBuffer.h"

using namespace easycppsockets;

SocketBuffer::SocketBuffer(Socket *socket)
    : socket_{socket}, readArea_{}, writeArea_{}
{
}

int SocketBuffer::underflow()
{
    /*
     * underflow() fn is called once gtr() == egptr, that is there is no data to be read.
     * Even though we double check that condition.
     */
    if (gptr() < egptr())
    {
        return traits_type::to_int_type(*gptr());
    }

    // Block until available
    ssize_t bytesRead = 0;
    try
    {
        bytesRead = socket_->recv(readArea_.data(), kBufferSize);
        if (bytesRead == 0)
        {
            throw std::runtime_error{"peer is closed"};
        }
    }
    catch (const std::runtime_error &e)
    {
        return traits_type::eof();
    }

    // Update the std::streambuff pointers
    setg(readArea_.data(), readArea_.data(), readArea_.data() + bytesRead);

    return traits_type::to_int_type(*gptr());
}

int SocketBuffer::overflow(int ch)
{
    size_t bytesToWrite = pptr() - pbase();

    if (bytesToWrite > 0)
    {
        try
        {
            if (socket_->send(pbase(), bytesToWrite) == 0)
            {
                throw std::runtime_error{"peer is closed"};
            }
        }
        catch (const std::runtime_error &e)
        {
            return traits_type::eof();
        }
    }

    // Reset the write pointers
    setp(writeArea_.data(), writeArea_.data() + kBufferSize);

    // Put in the overflowed character.
    if (!traits_type::eq_int_type(ch, traits_type::eof()))
    {
        sputc(ch);
    }

    // Return a non eof character to indicate success
    return traits_type::not_eof(ch);
}