#include <iostream>
#include <filesystem>
#include <signal.h>
#include "server.hpp"
#include "filesys.hpp"
#include "prog_options.hpp"
#include "loguru.hpp"
#include "init.hpp"
#include "blacklist.hpp"

namespace fs = std::filesystem;

std::string executable_file_name;
ProgOptions prog_options;

int main (int argc, char *argv[])
{
    if (prog_options.parse(argc, argv) == false) {
        return 1;
    }

    init_loguru (argc, argv);
    init_db (argc, argv);
    init_blacklist ();

    //set working directory
    if (prog_options.dir != ".") {
        std::error_code err, ok;
        //set working directory
        fs::current_path (prog_options.dir, err);
        if (err != ok) {
            LOG_S(ERROR) << "error changing working directory. dir=" << 
                prog_options.dir << " message = " << 
                err.message();
            return 1;
        }
    }
    else {
        //update the working directory
        prog_options.dir = fs::current_path().string();
    }

    prog_options.print_values ();
    executable_file_name = argv[0];

    signal(SIGPIPE, SIG_IGN);
    server_loop (prog_options.port, prog_options.tcp_backlog_size, 
        prog_options.tpool_size);

    return 0;
}
