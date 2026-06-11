#include <thread>
#include <vector>
#include <chrono>

#include "EasyCPPSockets/ServerSocket.h"
#include "EasyCPPSockets/Socket.h"
#include "gtest/gtest.h"

namespace
{

  using namespace easycppsockets;
  using namespace std::chrono_literals;

  TEST(ConnectionTestCase, SimpleSingleClientToServerConnection)
  {
    const int port = 3000;
    ServerSocket server{port, 1};

    auto clientThread = std::thread{[]()
                                    {
                                      Socket serverConnection{"127.0.0.1", port};
                                    }};

    auto clientConnection = server.accept();
    clientThread.join();
  }

  TEST(ConnectionTestCase, MultipleClientsToServerConnection)
  {
    const int port = 3000;
    int numClients = 1000;
    ServerSocket server{port, numClients};

    std::vector<std::thread> clientThreads;
    clientThreads.reserve(numClients);

    // Raise client connections in differente threads
    for (int i = 0; i < numClients; i++)
    {
      clientThreads.emplace_back([]()
                                 { EXPECT_NO_THROW(Socket serverConnection("127.0.0.1", port)); });
    }

    // Accept incoming connections
    for (int i = 0; i < numClients; i++)
    {
      EXPECT_NO_THROW(auto clientConnection = server.accept());
    }

    // Free resource
    for (int i = 0; i < numClients; i++)
    {
      clientThreads.at(i).join();
    }
  }

  TEST(ConnectionTestCase, SocketTimeout)
  {
    constexpr int port = 3000;
    ServerSocket server{port, 1};

    std::thread clientThread{[port]()
    {
      Socket serverConnection{"127.0.0.1", port};
      std::this_thread::sleep_for(3s);
    }};

    // Open the client conn
    auto clientConnection = server.accept();
    clientConnection->setOsTimeout(1s);

    // wait until the timeout runs out
    char response[1024];
    EXPECT_THROW(clientConnection->recv(response, 1024), std::runtime_error);
    clientThread.join();
  }
}