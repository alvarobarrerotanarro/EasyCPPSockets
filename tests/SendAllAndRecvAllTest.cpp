#include <iostream>
#include <vector>
#include <cstring>
#include <thread>
#include <cstdint>
#include <cstdlib>

#include "EasyCPPSockets/ServerSocket.h"
#include "EasyCPPSockets/Socket.h"

#include "gtest/gtest.h"

namespace
{
    using namespace easycppsockets;

    typedef struct Person
    {
        const char *name;
        int age;

        Person() : name{""}, age{0} {}

        Person(const char *name, int age) : name{name}, age{age}
        {
        }

        friend std::ostream &operator<<(std::ostream &os, const Person &p)
        {
            return os << "Person{" << p.name << ", " << p.age << "}";
        }
    } Person;

    TEST(SendAllAndRecvAllTest, SendAndReceiveSinglePerson)
    {
        const int port = 3000;
        ServerSocket server{port, 1};

        Person personToTransfer{"Alvaro Barrero", 18};
        bool success = false;

        std::thread clientThread([&personToTransfer, &success]()
                                 {
            Socket serverConnection{"127.0.0.1", port};

            Person receivedPerson;
            ssize_t bytesRead = serverConnection.recvAll(&receivedPerson, sizeof(Person));

            success = bytesRead == sizeof(Person) &&
                std::memcmp(&receivedPerson, &personToTransfer, sizeof(Person)) == 0;   
            });

        auto clientConnection = server.accept();
        clientConnection->sendAll(&personToTransfer, sizeof(Person));

        clientThread.join();

        EXPECT_EQ(success, true);
    }

    TEST(SendAllAndRecvAllTest, SendAndReceivePeople)
    {
        const int port = 3000;
        int peopleCount = 100;
        ServerSocket server{port, 1};
        bool success = false;

        // Load people
        std::srand(static_cast<unsigned long>(std::time(nullptr)));
        std::vector<Person> people;
        const char *names[] = {
            "Alice",
            "Bob",
            "Charlie",
            "David",
            "Emma",
            "Frank",
            "Grace",
            "Henry",
            "Irene",
            "Jack",
            "Alice",
            "Bob",
            "Charlie",
            "David",
            "Emma",
            "Frank",
            "Grace",
            "Henry",
            "Irene",
            "Jack",
            "Alice",
            "Bob",
            "Charlie",
            "David",
            "Emma",
            "Frank",
            "Grace",
            "Henry",
            "Irene",
            "Jack",
            "Alice",
            "Bob",
            "Charlie",
            "David",
            "Emma",
            "Frank",
            "Grace",
            "Henry",
            "Irene",
            "Jack",
            "Alice",
            "Bob",
            "Charlie",
            "David",
            "Emma",
            "Frank",
            "Grace",
            "Henry",
            "Irene",
            "Jack",
            "Alice",
            "Bob",
            "Charlie",
            "David",
            "Emma",
            "Frank",
            "Grace",
            "Henry",
            "Irene",
            "Jack",
            "Alice",
            "Bob",
            "Charlie",
            "David",
            "Emma",
            "Frank",
            "Grace",
            "Henry",
            "Irene",
            "Jack",
            "Alice",
            "Bob",
            "Charlie",
            "David",
            "Emma",
            "Frank",
            "Grace",
            "Henry",
            "Irene",
            "Jack",
            "Alice",
            "Bob",
            "Charlie",
            "David",
            "Emma",
            "Frank",
            "Grace",
            "Henry",
            "Irene",
            "Jack",
            "Alice",
            "Bob",
            "Charlie",
            "David",
            "Emma",
            "Frank",
            "Grace",
            "Henry",
            "Irene",
            "Jack",
        };
        for (int i = 0; i < peopleCount; i++)
        {
            people.emplace_back(names[i], std::rand() % 100);
        }

        std::thread clientThread{[peopleCount, &success, &people]()
        {
            Socket serverConnection{"127.0.0.1", port};

            Person *receivedPeople = new Person[peopleCount];
            ssize_t bytesRead = serverConnection.recvAll(receivedPeople, sizeof(Person) * peopleCount);

            success = bytesRead == sizeof(Person) * peopleCount &&
                std::memcmp(receivedPeople, people.data(), sizeof(Person) * peopleCount) == 0;

            delete[] receivedPeople;
        }};


        auto clientConnection = server.accept();
        clientConnection->sendAll(people.data(), sizeof(Person) * peopleCount);

        clientThread.join();
        EXPECT_EQ(success, true);
    }
}