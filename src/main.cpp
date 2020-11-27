#include <iostream>
#include "main.hpp"
#include "tcp.hpp"

int main (int argc, char *argv[])
{
    std::cout << "Hello world" << std::endl;

    TCPServer server (5000);
    while (1) {
        auto connection = server.accept_connection ();
        while (1) {
            ;
        }
    }
    

    return 0;
}