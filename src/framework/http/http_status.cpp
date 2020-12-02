#include "http_status.hpp"
#include <assert.h>


std::string 
http :: to_string (
    HTTPStatus status)
{
    std::string ret;

    switch (status) {
    //100 series

    //200 series
    case HTTPStatus::HTTP_OK:
        ret = "200 OK";
        break;
    
    //400 series
    case HTTPStatus::HTTP_BAD_REQUEST:
        ret = "400 Bad Request";
        break;
    
    case HTTPStatus::HTTP_NOT_FOUND:
        ret = "404 Not Found";
        break;
    
    case HTTPStatus::HTTP_UNSUPP_METHOD:
        ret = "405 Method Not Allowed";
        break;
    
    case HTTPStatus::HTTP_GEN_ERR:
        assert (false);
        ret = "HTTP_GEN_ERR";
        break;
    }

    return ret;
}

int 
http :: to_int (
    HTTPStatus status)
{
    int ret=0;

    switch (status) {
    //100 series

    //200 series
    case HTTPStatus::HTTP_OK:
        ret = 200;
        break;
    
    //400 series
    case HTTPStatus::HTTP_BAD_REQUEST:
        ret = 400;
        break;
    
    case HTTPStatus::HTTP_NOT_FOUND:
        ret = 404;
        break;
    
    case HTTPStatus::HTTP_UNSUPP_METHOD:
        ret = 405;
        break;
    
    case HTTPStatus::HTTP_GEN_ERR:
        assert (false);
        ret = 999;
        break;
    }

    return ret;
}