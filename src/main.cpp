#include <iostream>
#include "server.hpp"
#include "filesys.hpp"

std::string executable_file_name;

int main (int argc, char *argv[])
{
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <port_number>" << std::endl;
        return 1;
    }

    int port = std::stoi (argv[1]);

    executable_file_name = argv[0];
    server_loop (port);

    return 0;
}