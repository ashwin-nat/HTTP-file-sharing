#include "filesys.hpp"
#include "http.hpp"
#include <iostream>
#include <sstream>
#include <vector>

static void _fill_html_body (std::stringstream &ss, std::vector<FSEntry> &tree,
        std::string &uri);
static std::string _get_parent_dir (std::string &path);

HTTPStatus 
build_rsp_html (
    std::vector<char> &buffer, 
    HTTPRequest &req,
    std::vector <FSEntry> &tree)
{
    HTTPStatus ret = HTTPStatus::HTTP_OK;
    std::stringstream ss;
    ss << "<html>" << CHAR_LF;

    ss << "<head>" << CHAR_LF;
    ss << "<meta http-equiv=\"Content-Type\" content=\"text/html; " << 
        "charset=utf-8\">" << CHAR_LF;
    ss << "<title>Ashwin Server</title>" << CHAR_LF;
    ss << "</head>" << CHAR_LF;

    ss << "<body>" << CHAR_LF;
    ss << "<h1> Directory Structure for " << req.m_uri << "</h1>" << CHAR_LF;

    auto parent = (req.m_uri != "/" ) ? (_get_parent_dir (req.m_uri)) : 
            ("");
    //add link to parent directory, if needed
    if (!parent.empty()) {
        ss << "<a href=\"" << parent << "\">";
        ss << "Go UP: " << parent << "</a>" << CHAR_LF;
        ss << "<br>" << CHAR_LF;
    }

    ss << "<hr>";
    _fill_html_body (ss, tree, req.m_uri);

    ss << "<hr>" << CHAR_LF;
    ss << "</body>" << CHAR_LF;
    ss << "</html>" << CHAR_LF;

    for (auto &ch : ss.str()) {
        buffer.push_back (ch);
    }

    return ret;
}

void 
get_404_page (
    HTTPRequest &req, 
    std::vector<char> &buffer)
{
    std::stringstream ss;
    ss << "<html>" << CHAR_LF;

    ss << "<head>" << CHAR_LF;
    ss << "<meta http-equiv=\"Content-Type\" content=\"text/html; " << 
        "charset=utf-8\">" << CHAR_LF;
    ss << "<title>Ashwin Server</title>" << CHAR_LF;
    ss << "</head>" << CHAR_LF;

    ss << "<h1> 404 - Not Found " << "</h1>" << CHAR_LF;

    ss << "<hr>";
    ss << "Resource " << req.m_uri << " not found" << "<br>" << CHAR_LF;
    ss << "<a href=\"/\"" << "> Home page" << "</a> <br>" << CHAR_LF;

    ss << "<hr>" << CHAR_LF;
    ss << "</body>" << CHAR_LF;
    ss << "</html>" << CHAR_LF;

    for (auto &ch : ss.str()) {
        buffer.push_back (ch);
    }
}

static void 
_fill_html_body (
    std::stringstream &ss,
    std::vector<FSEntry> &tree,
    std::string &uri)
{
    ss << "<ul style=\"list-style-type:none;\">" << CHAR_LF;
    //prefix the links with the URI and ensure that it does not end with /
    std::string prefix = uri;
    if (prefix[prefix.length()-1] != '/') {
        prefix += '/';
    }
    
    //add every item to the unordered list
    for (auto &it : tree) {
        ss << "<li>";
        ss << "<a href=\"" << prefix + it.name << "\">" << it.name;
        //add / to the end of directory names
        if (it.is_dir) {
            ss << "/";
        }
        ss << "</a> </li>" << CHAR_LF;
    }

    ss << "</ul>" << CHAR_LF;
}

static std::string _get_parent_dir (std::string &path)
{
    //assumes the path is not /
    std::string ret;
    unsigned int max=0, index=0;

    //find the last / (not including the terminating /)
    if (path[path.size()-1] == '/') {
        max = path.size()-2;
    }
    else {
        max = path.size()-1;
    }
    for (; max>=0; --max) {
        if (path[max] == '/') { break; }
    }

    std::cout << __func__ << ": max=" << max << std::endl;
    //copy all characters upto that
    for (index=0; index<=max; ++index) {
        ret += path[index];
    }
    return ret;
}