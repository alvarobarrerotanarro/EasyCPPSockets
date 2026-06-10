#include <thread>
#include <cstring>
#include <vector>
#include <chrono>

#include "EasyCPPSockets/ServerSocket.h"
#include "EasyCPPSockets/Socket.h"
#include "gtest/gtest.h"

namespace
{
    using namespace easycppsockets;

    TEST(BasicSendAndReceiveTest, ClientGreetsServer)
    {
        const int port = 3000;
        ServerSocket server{port, 1};

        char message[] = "EasyCPPSockets";
        constexpr int messageLength = sizeof(message);

        std::thread clientThread{[&message]()
                                 {
                                     Socket serverConnection{"127.0.0.1", port};
                                     serverConnection.send(message, messageLength);
                                 }};

        auto clientConnection = server.accept();
        char echo[messageLength] = {0};
        clientConnection->recv(echo, messageLength);

        EXPECT_STREQ(message, echo);

        clientThread.join();
    }

    TEST(ReadAndWriteTest, MultipleClientsGreetServer)
    {
        const int port = 3000;
        int numClients = 1000;
        ServerSocket server{port, numClients};

        std::vector<std::thread> clientThreads;
        clientThreads.reserve(numClients);

        char message[] = "EasyCPPSockets";
        constexpr int messageLength = sizeof(message);

        for (int i = 0; i < numClients; i++)
        {
            clientThreads.emplace_back([&message]()
                                       {
                Socket serverConnection{"127.0.0.1", port};
                serverConnection.send(message, messageLength); });
        }

        char echo[messageLength] = {0};
        for (int i = 0; i < numClients; i++)
        {
            auto clientConnection = server.accept();
            clientConnection->recv(echo, messageLength);
            EXPECT_STREQ(message, echo);
        }

        for (int i = 0; i < numClients; i++)
        {
            clientThreads.at(i).join();
        }
    }

    TEST(ReadAndWriteTest, PingPong)
    {
        using namespace std::chrono_literals;

        const int port = 3000;
        int numPings = 10;
        ServerSocket server(port, 1);

        std::thread clientThread{[]()
        {
            Socket serverConnection{"127.0.0.1", port};

            bool exit = false;
            while (!exit)
            {
                char response[5] = {0};
                ssize_t bytesRead = serverConnection.recv(response, 4);
                response[4] = '\0';

                if (bytesRead != 4 || std::strcmp("ping", response) != 0) {
                    exit = true;
                } else {
                    char message[] = "pong";
                    serverConnection.send(message, 4);
                }
            }
        }};

        auto clientConnection = server.accept();
        for (int i = 0; i < numPings; i++) {
            char message[] = "ping";
            clientConnection->send(message, 4);

            char response[5] = {0};
            ssize_t bytesRead = clientConnection->recv(response, 4);
            response[4] = '\0';

            if (i == numPings - 1) {
                std::memcpy(message, "exit", 4);
                clientConnection->send(message, 4);
            } else {
                std::this_thread::sleep_for(250ms);
            }
        }

        clientThread.join();
    }
}