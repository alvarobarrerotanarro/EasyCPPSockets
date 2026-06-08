#include <iostream>
#include <stdexcept>

#include "ServerSocket.h"
#include "Socket.h"

using namespace easycppsockets;

int main()
{
    Socket socket{"127.0.0.1", 3000};
    bool cont = true;

    try
    {
        do
        {
            std::string line;
            socket.getSockStream() >> line;

            if (line == "ping")
            {
                std::cout << "pong" << '\n';
                socket.getSockStream() << "pong" << std::endl;
            }
            else if (line.empty())
            {
                cont = false;
            }

        } while (cont);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }
}