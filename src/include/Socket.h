#pragma once

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

#include "ServerSocket.h"

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

class Socket
{
    friend class ServerSocket;

private:
    static const int INPUT_BUFFER_MAX_LENGTH = 4096;

    std::vector<char> inputBuffer;
    size_t inputBufferPos;

    SocketDescriptor descriptor;

    /*
     * Server to client socket.
     */
    explicit Socket(SocketDescriptor &&descriptor)
        : descriptor{std::move(descriptor)}, inputBuffer{}, inputBufferPos{0}
    {
        inputBuffer.resize(INPUT_BUFFER_MAX_LENGTH);
    }

    void fillInputBuffer(char delimiter)
    {
        bool bufferHasSpace = (INPUT_BUFFER_MAX_LENGTH - 1) - inputBufferPos - 1 > 0;
        bool isDelimiter = memmem(inputBuffer.data(), inputBufferPos, &delimiter, 1) != nullptr;
        bool peerIsClosed = false;
        ssize_t bytesRead = -1;

        while (bufferHasSpace && !isDelimiter && !peerIsClosed)
        {
            // Repeat the read process if it is interrupted
            do
            {
                bytesRead = recv(descriptor, inputBuffer.data() + inputBufferPos, (INPUT_BUFFER_MAX_LENGTH - 1) - inputBufferPos - 1, 0);
            } while (bytesRead == -1 && errno == EINTR);

            // Check for errors
            if (bytesRead == -1 && errno != EINTR)
            {
                throw std::runtime_error{"Socket read failed"};
            }
            // Check for peer closed
            else if (bytesRead == 0)
            {
                peerIsClosed = true;
                inputBuffer[inputBufferPos + bytesRead] = '\n';
            }
            else
            {
                isDelimiter = memmem(inputBuffer.data() + inputBufferPos, bytesRead, &delimiter, 1) != nullptr;
                bufferHasSpace = INPUT_BUFFER_MAX_LENGTH - inputBufferPos - 2 > 0;

                if (!bufferHasSpace)
                {
                    inputBuffer[INPUT_BUFFER_MAX_LENGTH - 1] = '\n';
                }
                inputBufferPos += bytesRead;
            }
        }
    }

    /**
     * Assumes the existance of a delimiter previous to the inputBufferPos.
     */
    void shiftInputBuffer(char delimiter)
    {
        size_t delimiterPos = (char *)memmem(inputBuffer.data(), inputBufferPos, &delimiter, 1) - inputBuffer.data();
        size_t contentSize = inputBufferPos - delimiterPos - 1;
        memmove(inputBuffer.data(), inputBuffer.data() + delimiterPos + 1, contentSize);
        inputBufferPos = contentSize;
    }

public:
    /*
     * Client to server socket.
     */
    Socket(std::string serverIp, std::uint16_t serverPort)
        : descriptor{-1}, inputBuffer{}, inputBufferPos{0}
    {
        inputBuffer.resize(INPUT_BUFFER_MAX_LENGTH);

        // Open socket FD
        descriptor = socket(AF_INET, SOCK_STREAM, 0);
        if (descriptor == -1)
        {
            throw std::runtime_error{"Client socket open failed"};
        }

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

    Socket(const Socket &other) = delete;
    Socket(Socket &&other) noexcept
        : descriptor{-1}
    {
        this->descriptor = std::move(other.descriptor);
        this->inputBuffer = std::move(other.inputBuffer);
        this->inputBufferPos = other.inputBufferPos;

        other.inputBufferPos = 0;
    }

    Socket &operator=(const Socket &other) = delete;
    Socket &operator=(Socket &&other) noexcept
    {
        if (this != &other)
        {
            this->descriptor = std::move(other.descriptor);
            this->inputBuffer = std::move(other.inputBuffer);
            this->inputBufferPos = other.inputBufferPos;

            other.inputBufferPos = 0;
        }

        return *this;
    }

    /**
     * Extract a line from reading buffer.
     */
    std::string readLine(char delimiter)
    {
        // Fill the internal buffer
        fillInputBuffer(delimiter);

        // Find the delimiter
        char *delimiterOcurrence = (char *)memmem(inputBuffer.data(), inputBufferPos, &delimiter, 1);
        if (delimiterOcurrence == nullptr) // End of connection and thus no delimiter
        {
            return std::string{};
        }
        else
        {
            size_t lineLength = delimiterOcurrence - inputBuffer.data();

            // Create the std::string and shift the buffer
            std::string line{inputBuffer.data(), lineLength};
            shiftInputBuffer(delimiter);

            return line;
        }
    }

    void write(const std::string &bufferToWrite)
    {
        ssize_t bufferToWritePos = 0;
        ssize_t bytesWritten = -1;

        while (bufferToWritePos < bufferToWrite.size())
        {
            // Repeat the send process if it is interrupted
            do
            {
                bytesWritten = send(descriptor, bufferToWrite.data() + bufferToWritePos, bufferToWrite.size() - bufferToWritePos, 0);
            } while (bytesWritten == -1 && errno == EINTR);

            if (bytesWritten == -1 && errno != EINTR)
            {
                throw std::runtime_error{"Socket write failed"};
            }

            bufferToWritePos += bytesWritten;
        }
    }

    virtual ~Socket()
    {
    }
};