#include <thread>
#include <vector>
#include <chrono>
#include <stdexcept>

#include "EasyCPPSockets/ServerSocket.h"
#include "EasyCPPSockets/Socket.h"
#include "gtest/gtest.h"

namespace
{

  using namespace easycppsockets;
  using namespace std::chrono_literals;

  TEST(ConnectionTestCase, SimpleSingleClientToServerConnection)
  {
    ServerSocket server{0, 1};
    std::uint16_t port = server.getPort();

    auto clientThread = std::thread{[port]()
    {
      Socket serverConnection = Socket{"127.0.0.1", port};
    }};

    auto clientConnection = server.accept();
    clientThread.join();
  }

  TEST(ConnectionTestCase, GetSocketAddressAndPort)
  {
    ServerSocket server{0, 1};
    std::uint16_t port = server.getPort();

    bool clientSuccess = false;
    bool serverSuccess = false;

    std::string serverPresentationAddress = server.getPresentationAddress();
    std::uint16_t serverPort = server.getPort();

    std::thread clientThread{[port, &clientSuccess] () {
    Socket conn{"127.0.0.1", port};
    std::string clientPresentationAddress = conn.getPresentationAddress();
    std::uint16_t clientPort = conn.getPort();
    
    clientSuccess = clientPort != port &&
      clientPresentationAddress == "127.0.0.1";
    }};


    serverSuccess = serverPort == port &&
      serverPresentationAddress == "0.0.0.0";
    
    clientThread.join();

    EXPECT_TRUE(clientSuccess && serverSuccess);
  }

  TEST(ConnectionTestCase, MultipleClientsToServerConnection)
  {
    int numClients = 1000;
    ServerSocket server{0, numClients};
    std::uint16_t port = server.getPort();

    std::vector<std::thread> clientThreads;
    clientThreads.reserve(numClients);

    // Raise client connections in differente threads
    for (int i = 0; i < numClients; i++)
    {
      clientThreads.emplace_back([port]()
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
    ServerSocket server{0, 1};
    std::uint16_t port = server.getPort();

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