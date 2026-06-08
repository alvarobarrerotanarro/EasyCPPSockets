#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cmath>

#include "ServerSocket.h"
#include "Socket.h"

void ping(Socket &socket)
{
    char ping[] = "ping\n";
    if (socket.sendAll(ping, 5) == -1)
    {
        std::cerr << "send failed: " << strerror(errno) << '\n';
        std::exit(EXIT_FAILURE);
    }
    std::cout << ping;
}

bool isPong(Socket &socket)
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

        if (line == "pong")
        {
            isSuccess = true;
        }
    }

    return isSuccess;
}

int main()
{
    ServerSocket server{3000, 10};

    try
    {
        while (true)
        {
            std::unique_ptr<Socket> socket = server.accept();

            bool cont = true;
            while (cont)
            {

                std::cout << "SERVER: waiting to send ping" << '\n';
                ping(*socket);
                std::cout << "SERVER: server sent ping" << '\n';

                std::cout << "SERVER: waiting for pong" << '\n';
                if (!isPong(*socket))
                {
                    cont = false;
                }
                else
                {
                    std::cout << "SERVER: server got pong" << '\n';
                }

                std::this_thread::sleep_for(std::chrono::milliseconds{500});
            }
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }

    return EXIT_SUCCESS;
}