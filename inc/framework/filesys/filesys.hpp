#ifndef __FILESYS_HPP__
#define __FILESYS_HPP__

#include <string>
#include <vector>

struct FSEntry{
public:
    bool is_dir;
    std::string name;
};

/**
 * @brief       - Get the file listing for this directory
 * 
 * @param vec 
 * @return true 
 * @return false 
 */
bool 
get_file_listing (
    const std::string &path,
    std::vector<FSEntry> &vec);

/**
 * @brief       - Checks if this is a file or not
 * @param path  - string with the path
 * @return true - is a file
 * @return false- is NOT a file
 */
bool 
is_file (
    const std::string &path);

#endif  //__FILESYS_HPP__