#include <iostream>
#include <filesystem>
#include "server.hpp"
#include "filesys.hpp"
#include "prog_options.hpp"

namespace fs = std::filesystem;

std::string executable_file_name;
ProgOptions prog_options;

int main (int argc, char *argv[])
{
    if (prog_options.parse(argc, argv) == false) {
        return 1;
    }
    // if (argc != 2) {
    //     std::cerr << "Usage: " << argv[0] << " <port_number>" << std::endl;
    //     return 1;
    // }

    //set working directory
    if (prog_options.dir != ".") {
        std::error_code err, ok;
        //set working directory
        fs::current_path (prog_options.dir, err);
        if (err != ok) {
            std::cerr << "error changing working directory. message = " << 
                err.message() << std::endl;
            return 1;
        }
    }

    executable_file_name = argv[0];
    server_loop (prog_options.port, prog_options.tcp_backlog_size, 
        prog_options.tpool_size);

    return 0;
}