#include <iostream>
#include <thread>

#include "EasyCPPSockets/ServerSocket.h"
#include "EasyCPPSockets/Socket.h"

#include "gtest/gtest.h"

namespace {
    using namespace easycppsockets;

    TEST(SockStreamTest, HelloWorld) {
        ServerSocket server{0, 1};
        std::uint16_t port = server.getPort();

        std::thread clientThread{[port]() {
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