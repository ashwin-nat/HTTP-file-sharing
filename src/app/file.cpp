#include "http.hpp"
#include <iostream>
#include <sys/stat.h>
#include <regex>

static bool _is_text_file (const std::string &filename);
static inline std::string _clean_filename (const std::string &filename);

HTTPStatus 
get_file (
    HTTPResponse &rsp, 
    HTTPRequest &req)
{
    std::string resource;
    //skip first character since it's a /
    for (unsigned int index=1; index<req.m_uri.size(); ++index) {
        resource += req.m_uri[index];
    }

    const std::string &filename = _clean_filename(resource);

    struct stat stat_buf;
    if (stat (filename.c_str(), &stat_buf) == -1) {
        return HTTPStatus::HTTP_NOT_FOUND;
    }

    //update the filename in the response object
    rsp.m_rsp_filename = filename;
    rsp.m_len = stat_buf.st_size;

    //set content type
    if (_is_text_file (filename)) {
        rsp.m_content_type = "text/plain";
    }
    else {
        rsp.m_content_type = "application/octet-stream";
    }

    return HTTPStatus::HTTP_OK;
}

static bool 
_is_text_file (
    const std::string &filename)
{
    //find position of extension
    size_t pos = filename.rfind ('.');
    //if no extension, we'll consider it a text file
    if (std::string::npos == pos) {
        return true;
    }

    //build string with extension
    std::string extension;
    for (; pos<filename.size(); ++pos) {
        extension += filename[pos];
    }

    static const std::vector<std::string> supported_text_file_types = {
        ".txt", ".log", ".ini", ".cfg", ".conf", ".cpp", ".hpp",
        ".c", ".h", ".py", ".sh", ".java", ".js",
    };

    bool ret=false;
    for (auto &it : supported_text_file_types) {
        if (it == extension) {
            ret = true;
            break;
        }
    }

    return ret;
}

static inline
std::string 
_clean_filename (
    const std::string &filename)
{
    return std::regex_replace(filename, std::regex("%20"), " ");
}