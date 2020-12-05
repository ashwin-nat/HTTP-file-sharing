#include "prog_options.hpp"
#include <iostream>
#include <unistd.h>

#define PREFIX_SPACES       "    "

static void print_help (const char *bin_name);

bool 
ProgOptions :: parse (
    int argc,
    char *argv[])
{
    int opt = 0;
    extern char *optarg;
    bool unknown=false;
    /*
        supported options:
            -p: port number
            -d: working directory
            -v: verbose logging
            -t: thread pool worker count
            -h: help
            -b: TCP backlog size
    */

    while ((opt = getopt (argc, argv, "d:p:Vht:")) != -1) {
        switch (opt) {
        //set directory
        case 'd':
            dir = optarg;
            break;
        case 'p':
            port = std::stoi (optarg);
            break;
        case 'V':
            verbose = true;
            break;
        case 'h':
            print_help (argv[0]);
            break;
        case 't':
            tpool_size = std::stoi (optarg);
            break;
        case 'b':
            tcp_backlog_size = std::stoi (optarg);
            break;
        default:
            // std::cout << "unknown " << (char)opt << ": optind=" << optind 
            //     << " optarg=" << optarg << std::endl;
            unknown = true;
            break;
        }
    }
    return !unknown;
}

void 
ProgOptions :: print_values (void)
{
    if (verbose) {
        std::cout << "Starting HTTP file sharing server with" << std::endl;

        std::cout << PREFIX_SPACES << "Port = " << port << std::endl;
        std::cout << PREFIX_SPACES << "Directory = " << dir << std::endl;
        std::cout << PREFIX_SPACES << "TPool size = " << tpool_size 
            << std::endl;
        std::cout << PREFIX_SPACES << "TCP Backlog = " << tcp_backlog_size << 
            std::endl;
        std::cout << PREFIX_SPACES << "Log to console = " << log_to_console << 
            std::endl;
    }
}

static void 
print_help (
    const char *bin_name)
{
    std::cout << "Usage: " << bin_name << std::endl;

    std::cout << "The following arguments are optional" << std::endl;
    std::cout << PREFIX_SPACES << "Set server port number: -p <port_number>" 
        << std::endl;
    std::cout << PREFIX_SPACES << "Set server directory: " << 
        "-d <working_directory>" << std::endl;
    std::cout << PREFIX_SPACES << "Verbose logging: -V (Upper case)" 
        << std::endl;
    std::cout << PREFIX_SPACES << "Thread pool size: -t <size>" << std::endl;
    std::cout << PREFIX_SPACES << "TCP Backlog size: -b <size>" << std::endl;

    exit (0);
}