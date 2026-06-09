#include <thread>

#include "EasyCPPSockets/ServerSocket.h"
#include "EasyCPPSockets/Socket.h"
#include "gtest/gtest.h"

namespace {

  using namespace easycppsockets;

  TEST(ConnectionTestCase, SimpleSingleClientToServerConnection)
  { 
    const int port = 3000;
    ServerSocket server{port, 1}; 

    auto clientThread = std::thread{[]() {
      Socket serverConnection{"127.0.0.1", port};
    }};

    auto clientConnection = server.accept();
    clientThread.join();
  }

  TEST(ConnectionTestCase, MultipleClientsToServerConnection)
  {
    const int port = 3000;
    int numClients = 10;
    ServerSocket server{port, numClients};

    std::thread *clientThreads = new std::thread[numClients];

    // Raise client connections in differente threads
    for (int i = 0; i < numClients; i++) {
      clientThreads[i] = std::thread{[]() {
        EXPECT_NO_THROW(Socket serverConnection("127.0.0.1", port));
      }};
    }
    
    // Accept incoming connections
    for (int i = 0; i< numClients; i++) {
      EXPECT_NO_THROW(auto clientConnection = server.accept());
    }

    // Free resource
    for (int i = 0; i < numClients; i++) {
      clientThreads[i].join();
    }
    delete clientThreads;
  }
}