#ifndef __PROG_OPTIONS_HPP__
#define __PROG_OPTIONS_HPP__

#include <string>

struct ProgOptions {
    int port=80;
    std::string dir=".";
    bool verbose=false;
    unsigned int tpool_size=5;
    int tcp_backlog_size=5;
    std::string log_file;

    bool 
    parse (
        int argc, 
        char *argv[]);
    
    void 
    print_values (void);
};

#endif  //__PROG_OPTIONS_HPP__