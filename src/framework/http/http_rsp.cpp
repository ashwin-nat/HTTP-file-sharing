#include "http_rsp.hpp"
#include "http.hpp"
#include <sstream>

/**
 * @brief           - Copy to the response body
 * @param data      - void pointer to the start of the data
 * @param len       - len of the data
 * @return true     - data copied
 * @return false    - data NOT copied
 */
bool 
HTTPResponse :: set_body (
    const void *data,
    size_t len)
{
    auto ptr = const_cast<char*> (static_cast <const char*> (data));
    for (char *temp=ptr; temp < (ptr+len); ++temp) {
        m_body.push_back (*temp);
    }

    m_len = len;
    return true;
}

/**
 * @brief           - Frame a HTTP response with these parameters
 * @param buffer    - Buffer into which the response is to be written
 * @return true     - Successfully written into buffer
 * @return false    - failed to write into buffer
 */
bool 
HTTPResponse :: build_rsp (
    std::vector<char> &buffer)
{
    if (m_status == HTTPStatus::HTTP_GEN_ERR || 
            m_len == 0 || 
            m_content_type.empty() ||
            m_body.empty()) {
        
        return false;
    }

    std::string crlf;
    crlf += CHAR_CR;
    crlf += CHAR_LF;

    std::stringstream temp;

    //fill the version
    temp << "HTTP/";
    temp << SERVER_HTTP_VER;
    temp << CHAR_SPACE;

    //fill the status
    temp << http::to_string (m_status);
    temp << crlf;

    //fill the server info
    temp << "Server: ";
    temp << SERVER_INFO;
    temp << crlf;

    //notify that persistent connection is not supported
    temp << "Connection: close";
    temp << crlf;

    //content type
    temp << "Content-type: ";
    temp << m_content_type;
    temp << crlf;

    //content length
    temp << "Content-Length: ";
    temp << m_len;
    temp << crlf;

    //start of body
    temp << crlf;
    for (auto ch : m_body) {
        temp << ch;
    }

    //now, write everything into the provided buffer
    for (auto ch : temp.str()) {
        buffer.push_back (ch);
    }
    return true;
}