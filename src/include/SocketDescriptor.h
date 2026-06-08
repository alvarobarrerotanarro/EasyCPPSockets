#pragma once

#include <unistd.h>

class SocketDescriptor
{
private:
    int descriptor;

public:
    SocketDescriptor(int descriptor)
        : descriptor{descriptor}
    {
    }

    SocketDescriptor(const SocketDescriptor &other) = delete;

    SocketDescriptor(SocketDescriptor &&other) noexcept
    {
        this->descriptor = other.descriptor;
        other.descriptor = -1;
    }

    SocketDescriptor &operator=(const SocketDescriptor &other) = delete;
    SocketDescriptor &operator=(SocketDescriptor &&other) noexcept
    {
        if (this != &other)
        {
            if (this->descriptor != -1)
            {
                close(this->descriptor);
            }

            this->descriptor = other.descriptor;
            other.descriptor = -1;
        }

        return *this;
    }

    operator int()
    {
        return descriptor;
    }

    ~SocketDescriptor()
    {
        if (descriptor != -1)
        {
            close(descriptor);
        }
    }
};