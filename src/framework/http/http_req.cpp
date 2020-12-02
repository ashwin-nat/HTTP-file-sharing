#include "http_req.hpp"
#include "http.hpp"
#include <vector>
#include <sstream>
#include <iostream>
#include <iterator>

static std::pair<std::string,std::string> _split_line_colon (
    const std::string &input);


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

    for (auto &it: lines) {
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

        second += input[index];
    }

    return std::pair<std::string,std::string> (first, second);
}