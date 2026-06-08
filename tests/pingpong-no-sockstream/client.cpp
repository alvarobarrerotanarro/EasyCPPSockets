#include <iostream>
#include <stdexcept>

#include "ServerSocket.h"
#include "Socket.h"

void pong(Socket &socket)
{
    char pong[] = "pong\n";
    if (socket.sendAll(pong, 5) == -1)
    {
        std::cerr << "send failed: " << strerror(errno) << '\n';
        std::exit(EXIT_FAILURE);
    }
    std::cout << pong;
}

bool isPing(Socket &socket)
{
    bool isSuccess = false;

    char lineData[5] = {0};
    ssize_t bytesRead = socket.recvAll(lineData, 5);

    if (bytesRead == -1)
    {
        std::cerr << "recv failed: " << strerror(errno) << '\n';
        std::exit(EXIT_FAILURE);
    }
    else if (bytesRead == 0)
    {
        std::cerr << "peer closed the socket" << '\n';
        isSuccess = false;
    }
    else
    {
        std::string line{lineData, std::min(4L, bytesRead)};

        if (line == "ping")
        {
            isSuccess = true;
        }
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