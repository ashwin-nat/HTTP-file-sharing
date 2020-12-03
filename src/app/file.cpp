#include "http.hpp"
// #include <fstream>
#include <iostream>
// #include <sstream>
#include <sys/stat.h>

HTTPStatus 
get_file (
    HTTPResponse &rsp, 
    HTTPRequest &req)
{
    std::string filename;
    for (unsigned int index=1; index<req.m_uri.size(); ++index) {
        filename += req.m_uri[index];
    }
    //check if file exists
    // std::ifstream file (filename);
    // if (file.is_open()==false) {
    //     return HTTPStatus::HTTP_NOT_FOUND;
    // }

    struct stat stat_buf;
    if (stat (filename.c_str(), &stat_buf) == -1) {
        return HTTPStatus::HTTP_NOT_FOUND;
    }

    //update the filename in the response object
    rsp.m_rsp_filename = filename;
    rsp.m_len = stat_buf.st_size;

    // std::ostringstream ss;
    // ss << file.rdbuf();

    // for (auto &ch : ss.str()) {
    //     buffer.push_back (ch);
    // }

    return HTTPStatus::HTTP_OK;
}