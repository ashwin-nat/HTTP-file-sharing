#ifndef __HTTP_STATUS_HPP__
#define __HTTP_STATUS_HPP__

#include <string>

enum class HTTPStatus {
    //100 series

    //200 series
    HTTP_OK,

    //300 series

    //400 series
    HTTP_BAD_REQUEST,
    HTTP_UNSUPP_METHOD,
    HTTP_NOT_FOUND,

    //500 series

    //gen err
    HTTP_GEN_ERR,
};

namespace http {
    std::string 
    to_string (
        HTTPStatus status);
    
    int 
    to_int (
        HTTPStatus status);
};

#endif  //__HTTP_STATUS_HPP__