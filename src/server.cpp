#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "ServerSocket.h"
#include "Socket.h"

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
                std::cout << "ping" << '\n';
                socket->write("ping\n");
                std::string response = socket->readLine('\n');

                if (response != "pong")
                {
                    cont = false;
                }

                // std::this_thread::sleep_for(std::chrono::milliseconds{500});
            }
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }

    /*
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Server socket creation failed: " << errno << '\n';
        exit(1);
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(3000);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress));

    if (listen(serverSocket, 10) == -1) {
        std::cerr << "Server socket listen failed: " << errno << '\n';
        close(serverSocket);
        exit(1);
    }

    int clientSocket = accept(serverSocket, nullptr, nullptr);
    if (clientSocket == -1) {
        std::cerr << "Client accept failed: " << errno << '\n';
        close(serverSocket);
        exit(1);
    }

    char buffer[1024] = {0};
    if (recv(clientSocket, buffer, 1024, 0) == -1) {
        std::cerr << "Client socket read failed: " << errno << '\n';
        close(serverSocket);
        close(clientSocket);
        exit(1);
    }

    std::cout << "Message from client: " << buffer << '\n';
    */

    return 0;
}