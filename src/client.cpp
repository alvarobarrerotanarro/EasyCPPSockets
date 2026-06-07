#include <iostream>
#include <stdexcept>

#include "ServerSocket.h"
#include "Socket.h"

int main()
{
    Socket socket{"127.0.0.1", 3000};
    std::string line;

    try
    {
        do
        {
            line = socket.readLine('\n');

            if (line == "ping")
            {
                std::cout << "pong" << '\n';
                socket.write("pong\n");
            }

        } while (line.size() > 0);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }
}