#include "http_req.hpp"
#include "http.hpp"
#include <vector>
#include <sstream>
#include <iostream>
#include <iterator>

static std::pair<std::string,std::string> _split_line_colon (
    const std::string &input);
static std::tuple <std::string, std::string, std::string> 
    _parse_http_method (const std::string &input);


/**
 * @brief       - Parse the given HTTP request into this struct
 * @param lines - Vector of lines in the HTTP request
 * @return true - parsed successfully, valid/supported request
 * @return false- invalid/unsupported request
 */
bool 
HTTPRequest :: parse (
    const std::vector<std::string> &lines)
{
    std::string crlf;
    crlf += CHAR_CR;
    crlf += CHAR_LF;

    bool is_header = true;
    bool first_line = true;

    for (auto &it: lines) {
        if (first_line) {

            //first line is the method and its info
            first_line = false;
            auto temp = _parse_http_method (it);
            m_method = std::get<0> (temp);
            m_uri = std::get<1> (temp);
            m_ver = std::get<2> (temp);
        }
        else {
            //insert header into the map
            if (is_header) {
                if (it == crlf) {
                    is_header = false;
                }
                else {
                    auto temp = _split_line_colon (it);
                    m_header.insert (temp);
                }
            }
            //insert body into the vector
            else {
                m_body.push_back (it);
            }
        }
    }

    auto result = m_header.find ("Host");
    if (result != m_header.end()) {
        m_host = result->second;
    }
    return true;
}

/**
 * @brief       - Clears all members of this object
 */
void 
HTTPRequest ::clear (void)
{
    m_header.clear();
    m_body.clear();
}

static std::pair<std::string,std::string> 
_split_line_colon (
    const std::string &input)
{
    unsigned int index;
    std::string first, second;
    for (index = 0; index<input.length(); index++) {
        if (input[index] == CHAR_COLON) {
            index++;
            break;
        }
        else {
            first += input[index];
        }
    }

    //build the second string
    bool initial_spaces=true;
    for (; index <input.length(); index++) {
        //skip spaces after colon till start of text
        if (initial_spaces && input[index] == CHAR_SPACE) { continue; }

        //if characters begin, start storing them
        if (initial_spaces && input[index] != CHAR_SPACE) {
            initial_spaces = false;
        }

        if (!initial_spaces && input[index] == CHAR_CR) {
            break;
        }

        second += input[index];
    }

    return std::pair<std::string,std::string> (first, second);
}

static std::tuple <std::string, std::string, std::string> 
_parse_http_method (
    const std::string &input)
{
    std::string method,uri,ver;
    unsigned int index = 0;

    //get method
    for (; index<input.size(); ++index) {
        if (input[index] == CHAR_SPACE) {
            ++index;
            break;
        }
        method += input[index];
    }

    //get URI
    for (; index<input.size(); ++index) {
        if (input[index] == CHAR_SPACE) {
            ++index;
            break;
        }
        uri += input[index];
    }

    //get version
    for (; index<input.size(); ++index) {
        if (input[index] == CHAR_SPACE) {
            ++index;
            break;
        }
        ver += input[index];
    }

    return std::tuple<std::string,std::string,std::string> (method,uri,ver);
}