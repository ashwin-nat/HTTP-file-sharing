#include "filesys.hpp"
#include "prog_options.hpp"
#include "loguru.hpp"
#include <filesystem>
#include <iostream>
#include <limits.h>

#define _LAST_N_BITS_SET(n)             ((2 << ((n)-1))-1)

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
    auto status = _get_filesys_structure (temp_path, vec);
    if (!status) {
        return status;
    }

    //get the size of each file
    for (auto &it : vec) {
        if (!it.is_dir) {
            const std::string _abs_path = _get_cwd() + path + it.name;
            get_file_size (_abs_path, it.file_size);
        }
    }
    return true;
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

/**
 * @brief       - Get the sizeof the file
 * @param filepath - path to the file
 * @param result    - Ref to the variable where result is to be stored
 * @return true     - operation successful
 * @return false    - operation failure
 */
bool 
get_file_size (
    const std::string &filepath,
    size_t &result)
{
    std::error_code err, good;

    auto size = fs::file_size(filepath, err);
    if (err == good) {
        result = size;
        return true;
    }
    return false;
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

	//skip favicon file
	if (item.name == "favicon.ico") {
		continue;
	}
        //skip hidden files and this executable file
        if (item.name[0] != '.') {
            vec.push_back (item);
        }
    }
    return true;
}

/**
 * @brief               - Get a C++ string with human readable size
 * @param bytes         - the size to be formatted
 * @return std::string  - return string
 */
std::string 
bytes_human_readable (
    size_t bytes)
{
    static const std::vector<std::string> sizes = { "B", "kB", "MB", "GB", "TB"};
    size_t order = 0;
    size_t quot=bytes, rem=0;

    while (quot > 1024) {
        //bit shift for fun, basically rem = quot%1024
        rem = quot & (_LAST_N_BITS_SET(10));
        //bit shift for fun, basically quot = quot/1024
        quot = quot >> 10;
        order++;
    }

    std::string ret = std::to_string (quot) + "." + std::to_string (rem);
    if (order >= sizes.size()) {
        order = sizes.size()-1;
    }

    ret += " " + sizes[order];
    //if (order > 0) {
        //ret += " (" + std::to_string (bytes) + " B)";
    //}
    return ret;
}
