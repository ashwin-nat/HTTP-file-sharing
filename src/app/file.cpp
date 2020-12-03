#include "http.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

HTTPStatus 
get_file (
    std::vector<char> &buffer, 
    HTTPRequest &req)
{
    std::string filename;
    for (unsigned int index=1; index<req.m_uri.size(); ++index) {
        filename += req.m_uri[index];
    }
    //check if file exists
    std::ifstream file (filename);
    if (file.is_open()==false) {
        return HTTPStatus::HTTP_NOT_FOUND;
    }

    std::ostringstream ss;
    ss << file.rdbuf();

    for (auto &ch : ss.str()) {
        buffer.push_back (ch);
    }

    return HTTPStatus::HTTP_OK;
}