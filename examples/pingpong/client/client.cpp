#include <iostream>
#include <string>
#include <stdexcept>

#include "EasyCPPSockets/Socket.h"

using namespace easycppsockets;

int main()
{
    Socket socket{"127.0.0.1", 3000};
    std::cout << "Cient socket info " << socket.getPresentationAddress() << ":" << socket.getPort() << "'\n";

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
            else
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