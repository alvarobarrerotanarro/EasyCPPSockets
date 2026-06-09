#include <iostream>
#include <stdexcept>

#include "EasyCPPSockets/ServerSocket.h"
#include "EasyCPPSockets/Socket.h"

using namespace easycppsockets;

void pong(Socket &socket)
{
    char pong[] = "pong\n";
    try
    {
        socket.sendAll(pong, 5);
        std::cout << pong;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        std::exit(EXIT_FAILURE);
    }
}

bool isPing(Socket &socket)
{
    bool isSuccess = false;

    try
    {
        char lineData[5] = {0};
        ssize_t bytesRead = socket.recvAll(lineData, 5);
        if (bytesRead == 0)
        {
            std::cerr << "peer closed the socket" << '\n';
            isSuccess = false;
        }
        else
        {
            long lineLength = std::min(4L, bytesRead);
            std::string line{lineData, static_cast<std::string::size_type>(lineLength)};

            if (line == "ping")
            {
                isSuccess = true;
            }
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "recv failed: " << strerror(errno) << '\n';
        std::exit(EXIT_FAILURE);
    }

    return isSuccess;
}

int main()
{
    Socket socket{"127.0.0.1", 3000};
    bool cont = true;

    try
    {
        do
        {
            std::cout << "CLIENT: waiting for ping" << '\n';
            if (!isPing(socket))
            {
                cont = false;
            }
            else
            {
                std::cout << "SERVER: server got ping" << '\n';

                std::cout << "CLIENT: waiting to send" << '\n';
                pong(socket);
                std::cout << "CLIENT: client sent pong" << '\n';
            }

        } while (cont);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }
}