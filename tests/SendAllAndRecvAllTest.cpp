#include <iostream>
#include <vector>
#include <algorithm>
#include <array>
#include <cstring>
#include <thread>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <filesystem>
#include <atomic>

#include "EasyCPPSockets/ServerSocket.h"
#include "EasyCPPSockets/Socket.h"

#include "gtest/gtest.h"

namespace
{
    using namespace easycppsockets;
    namespace fs = std::filesystem;

    typedef struct Person
    {
        const char *name;
        int age;

        Person() : name{""}, age{0} {}

        Person(const char *name, int age) : name{name}, age{age}
        {
        }

        static Person parse(const char *str)
        {
            size_t finalPos = std::strlen(str);
            if (finalPos > 63)
            {
                throw std::runtime_error{"Failed to parse person. String length is over 63."};
            }

            std::ptrdiff_t numOcurrences = std::count(str, str + finalPos, ',');
            if (numOcurrences != 1)
            {
                throw std::runtime_error{std::string("Failed to parse person. One and only comma is expected: ") + str};
            }

            const char *ocurrence = std::find(str, str + finalPos, ',');
            std::ptrdiff_t nameLength = ocurrence - str + 1;
            std::ptrdiff_t ageLength = str + finalPos - ocurrence;

            auto name = std::make_unique<char[]>(nameLength);
            auto ageStr = std::make_unique<char[]>(ageLength);

            std::memcpy(name.get(), str, nameLength - 1);
            name[nameLength - 1] = '\0';
            std::memcpy(ageStr.get(), str + nameLength + 1, ageLength - 1);
            ageStr[ageLength - 1] = '\0';

            int age = std::atoi(ageStr.get());
            if (age == 0 && std::strcmp("0", ageStr.get()) != 0)
            {
                throw std::runtime_error{"Failed to parse person. Invalid numerical format for age."};
            }

            return {name.get(), age};
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
                std::memcmp(&receivedPerson, &personToTransfer, sizeof(Person)) == 0; });

        auto clientConnection = server.accept();
        clientConnection->sendAll(&personToTransfer, sizeof(Person));

        clientThread.join();

        EXPECT_EQ(success, true);
    }

    TEST(SendAllAndRecvAllTest, SendAndReceivePeopleMultipleClients)
    {
        // Configure the server
        const int port = 3000;
        int numClients = 1000;
        ServerSocket server{port, numClients};
        std::atomic<int> successCount = 0;
        // Configure the server

        // Load people (~750 kib * 1000 people = ~750,000 kib = ~730 MiB)
        const char *filePath = EASYCPPSOCKETS_SOURCE_DIR "/tests/people.csv";
        if (!fs::exists(filePath))
        {
            throw std::runtime_error{std::string{"Missing people csv file: "} + filePath};
        }
        std::ifstream peopleCsvFile(filePath);
        char personStr[64] = {0}; // Process header
        peopleCsvFile.getline(personStr, 63);

        std::vector<Person> people;
        while (peopleCsvFile)
        {
            peopleCsvFile.getline(personStr, 63);
            personStr[63] = '\0';

            if (personStr[0] != '\0') // There is content after the \n
            {
                people.push_back(Person::parse(personStr));
            }
        }
        // Load people

        // Start clients
        std::vector<std::thread> clientThreads;
        clientThreads.reserve(numClients);
        for (int i = 0; i < numClients; i++)
        {
            clientThreads.emplace_back([&people, &successCount]()
                                       {
                    Socket serverConnection{"127.0.0.1", port};

                    const size_t peopleCount = people.size();
                    Person *receivedPeople = new Person[peopleCount];

                    ssize_t bytesRead = serverConnection.recvAll(receivedPeople, sizeof(Person) * peopleCount);

                    successCount += bytesRead == sizeof(Person) * peopleCount &&
                            std::memcmp(receivedPeople, people.data(), sizeof(Person) * peopleCount) == 0 ? 1 : 0;

                    delete[] receivedPeople; });
        }
        // Start clients

        // Send people to clients
        for (int i = 0; i < numClients; i++)
        {
            auto clientConnection = server.accept();
            clientConnection->sendAll(people.data(), sizeof(Person) * people.size());
        }
        // Send people to clients

        // Wait for threads to finish
        for (int i = 0; i < numClients; i++)
        {
            clientThreads.at(i).join();
        }
        // Wait for threads to finish

        EXPECT_EQ(successCount, numClients);
    }
}