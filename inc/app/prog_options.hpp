#ifndef __PROG_OPTIONS_HPP__
#define __PROG_OPTIONS_HPP__

#include <string>

struct ProgOptions {
    int port=80;
    std::string dir=".";
    bool verbose=false;
    unsigned int tpool_size=5;
    int tcp_backlog_size=5;

    bool 
    parse (
        int argc, 
        char *argv[]);
};

#endif  //__PROG_OPTIONS_HPP__