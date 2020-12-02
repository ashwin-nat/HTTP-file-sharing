#ifndef __HTTP_REQ_HPP__
#define __HTTP_REQ_HPP__

#include <string>
#include <vector>
#include <map>

struct HTTPRequest {
public:
    std::string m_method;
    std::string m_uri;
    std::string m_ver;
    std::string m_host;
    std::map <std::string, std::string> m_header;
    std::vector <std::string> m_body;

    /**
     * @brief       - Parse the given HTTP request into this struct
     * @param lines - Vector of lines in the HTTP request
     * @return true - parsed successfully, valid/supported request
     * @return false- invalid/unsupported request
     */
    bool 
    parse (
        const std::vector<std::string> &lines);
    /**
     * @brief       - Clears all members of this object
     */
    void 
    clear (void);
};

#endif  //__HTTP_REQ_HPP__