#include <iostream>
#include <thread>

#include "EasyCPPSockets/ServerSocket.h"
#include "EasyCPPSockets/Socket.h"

#include "gtest/gtest.h"

namespace {
    using namespace easycppsockets;

    TEST(SockStreamTest, HelloWorld) {
        constexpr int port = 3000;
        ServerSocket server{port, 1};

        std::thread clientThread{[]() {
            Socket conn{"127.0.0.1", port};
            conn.getSockStream() << "Hello I'am a client. Never forget to flush the stream." << std::endl;
        }};

        auto clientConn = server.accept();
        std::string response{};
        clientConn->getSockStream() >> response;
        std::cerr << response << '\n';

        clientThread.join();
    }

}