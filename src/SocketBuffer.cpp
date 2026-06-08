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
    ssize_t bytesRead = socket_->recv(readArea_.data(), kBufferSize);
    if (bytesRead < 1)
    {
        return traits_type::eof();
    }

    // Update the std::streambuff pointers
    setg(readArea_.data(), readArea_.data(), readArea_.data() + bytesRead);

    return traits_type::to_int_type(*gptr());
}

int SocketBuffer::overflow(int ch)
{
    size_t bytesWritten = pptr() - pbase();

    if (bytesWritten > 0 && socket_->send(pbase(), bytesWritten) == -1)
    {
        return traits_type::eof(); // Send error
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