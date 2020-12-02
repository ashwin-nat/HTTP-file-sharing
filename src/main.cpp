#include <iostream>
#include "server.hpp"
#include "filesys.hpp"

int main (int argc, char *argv[])
{
    std::cout << "Hello world" << std::endl;
    // std::vector <FSEntry> tree;
    // get_file_listing ("/abc", tree);

    // for (auto &it : tree) {
    //     if (it.is_dir) {
    //         std::cout << "D: ";
    //     }
    //     else {
    //         std::cout << "F: ";
    //     }
    //     std::cout << it.name << std::endl;
    // }
    // return 0;


    server_loop (5000);

    return 0;
}