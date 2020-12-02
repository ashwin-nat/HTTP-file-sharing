#ifndef __PROCESS_REQUEST_HPP__
#define __PROCESS_REQUEST_HPP__

#include "http.hpp"

/**
 * @brief           - Process incoming HTTP request
 * @param lines     - vector of lines containing the HTTP request
 * @param connection- pointer to the connection object
 * @return true     - Request processed successfully
 * @return false    - request processing failed, drop the connection
 */
bool 
process_request (
    const std::vector<std::string> &lines,
    std::shared_ptr<TCPConnection> connection);

#endif  //__PROCESS_REQUEST_HPP__