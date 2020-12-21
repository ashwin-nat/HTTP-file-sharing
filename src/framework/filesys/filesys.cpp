#include "filesys.hpp"
#include "prog_options.hpp"
#include "loguru.hpp"
#include <filesystem>
#include <iostream>
#include <limits.h>

namespace fs = std::filesystem;

std::string _cook_path (const std::string &path);
std::string _get_cwd (void);
std::string _extract_name_from_path (const std::string &path);
bool _get_filesys_structure (std::string &temp_path,
        std::vector<FSEntry> &vec);
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
    std::vector<FSEntry> &vec)
{
    std::string temp_path = _cook_path (path);
    if (temp_path.size() >= PATH_MAX) {
        LOG_S(ERROR) << "Path is too long. len=" << temp_path.size();
        return false;
    }
    return _get_filesys_structure (temp_path, vec);
}

/**
 * @brief       - Checks if this is a file or not
 * @param path  - string with the path
 * @return true - is a file
 * @return false- is NOT a file
 */
bool 
is_file (
    const std::string &path)
{
    const fs::path fspath (_cook_path(path));
    std::error_code err;
    if (fs::is_regular_file(fspath, err)) {
        return true;
    }
    else {
        return false;
    }
}

std::string 
_cook_path (
    const std::string &path)
{
    return (_get_cwd() + path);
}

std::string
    _get_cwd (void)
{
    // return fs::current_path().string();
    //since we keep the updated working directory in prog_options
    extern ProgOptions prog_options;
    return prog_options.dir;
}

std::string
_extract_name_from_path (
    const std::string &path)
{
    unsigned int index;
    //find last /
    for (index=path.size()-1; index>=0; --index) {
        if (path[index] == '/') {
            ++index;
            break;
        }
    }
    std::string ret;
    for (; index<path.size(); ++index) {
        ret += path[index];
    }
    return ret;
}

bool
_get_filesys_structure (
    std::string &path,
    std::vector<FSEntry> &vec)
{
    extern std::string executable_file_name;
    FSEntry item;
    std::error_code err, ok;
    auto dir_iter = fs::directory_iterator(path, err);
    if (err != ok) {
        LOG_S(INFO) << "error " << err.message();
        return false;
    }
    for (auto &entry : dir_iter) {
        if (entry.is_directory()) {
            item.is_dir = true;
        }
        else {
            item.is_dir = false;
        }
        if (entry.path() == executable_file_name) {
            continue;
        }
        item.name = _extract_name_from_path (entry.path());
        //skip hidden files and this executable file
        if (item.name[0] != '.') {
            vec.push_back (item);
        }
    }
    return true;
}
