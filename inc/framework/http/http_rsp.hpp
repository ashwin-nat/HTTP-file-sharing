#ifndef __HTTP_RSP_HPP__
#define __HTTP_RSP_HPP__

#include "http_status.hpp"
#include <string>
#include <vector>

struct HTTPResponse {
public:
    HTTPStatus m_status= HTTPStatus::HTTP_GEN_ERR;
    size_t m_len=0;
    std::string m_content_type;
    std::vector<char> m_body;

    std::string m_rsp_filename;

    /**
     * @brief           - Copy to the response body
     * @param data      - void pointer to the start of the data
     * @param len       - len of the data
     * @return true     - data copied
     * @return false    - data NOT copied
     */
    bool 
    set_body (
        const void *data,
        size_t len);
    /**
     * @brief           - Frame a HTTP response with these parameters
     * @param buffer    - Buffer into which the response is to be written
     * @return true     - Successfully written into buffer
     * @return false    - failed to write into buffer
     */
    bool 
    build_rsp (
        std::vector<char> &buffer);
};

#endif  //__HTTP_RSP_HPP__