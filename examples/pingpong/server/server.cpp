#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "EasyCPPSockets/ServerSocket.h"
#include "EasyCPPSockets/Socket.h"

using namespace easycppsockets;

int main()
{
    ServerSocket server{3000, 1};
    std::cout << "Server at '" << server.getPresentationAddress() << ":" << server.getPort() << "'\n";

    try
    {
        while (true)
        {
            std::unique_ptr<Socket> socket = server.accept(); 
            std::cout << "New client " << socket->getPresentationAddress() << ":" << socket->getPort() << "'\n";

            bool cont = true;
            while (cont)
            {
                std::cout << "ping" << '\n';
                socket->getSockStream() << "ping" << std::endl;

                std::string line;
                socket->getSockStream() >> line;

                if (line == "pong")
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds{500});
                }
                else
                {
                    cont = false;
                }
            }
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }

    return 0;
}