#include "filesys.hpp"
#include "http.hpp"
#include <iostream>
#include <sstream>
#include <vector>
#include <regex>

static void _fill_html_body (std::stringstream &ss, std::vector<FSEntry> &tree,
        std::string &uri);
static std::string _get_parent_dir (std::string &path);
static void _get_back_links (std::stringstream &ss, std::string &parent);
static void __get_vector_of_path_nodes (std::string &parent, 
        std::vector<std::string> &path_nodes);
static void __fill_src_addr (std::stringstream &ss, const std::string &src);
static const std::string _make_http_friendly (const std::string &filename);

HTTPStatus 
build_rsp_html (
    std::vector<char> &buffer, 
    HTTPRequest &req,
    std::vector <FSEntry> &tree,
    const std::string &src)
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
        _get_back_links (ss, parent);
    }

    ss << "<hr>";
    _fill_html_body (ss, tree, req.m_uri);

    ss << "<hr>" << CHAR_LF;

    __fill_src_addr (ss, src);

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
    std::vector<char> &buffer,
    const std::string &src)
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

    __fill_src_addr (ss, src);

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
    
    //if directory is empty, add 2 empty lines
    if (tree.empty()) {
        ss << "<br> <br>" << CHAR_LF;
        return;
    }

    //add every item to the unordered list
    for (auto &it : tree) {
        const std::string &filename = _make_http_friendly (it.name);

        ss << "<li>";
        ss << "<a href=\"" << prefix + filename << "\">" << it.name;
        //add / to the end of directory names
        if (it.is_dir) {
            ss << "/";
        }
        //add file size to files
        else {
            const std::string &_size_str = bytes_human_readable (it.file_size);
            ss << " (" << _size_str << ")";
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

    //copy all characters upto that
    for (index=0; index<=max; ++index) {
        ret += path[index];
    }
    return ret;
}

static void 
_get_back_links (
    std::stringstream &ss, 
    std::string &parent)
{
    // ss << "<a href=\"" << parent << "\">";
    // ss << "Go UP: " << parent << "</a>" << CHAR_LF;
    // ss << "<br>" << CHAR_LF;

    std::vector<std::string> path_nodes;
    __get_vector_of_path_nodes (parent, path_nodes);

    ss << "<a href=\"" << "/" << "\">";
    ss << "Go UP: " << "/" << "</a>" << CHAR_LF;
    // ss << "<br>" << CHAR_LF;

    std::string path = "/";
    for (auto &it : path_nodes) {
        path += it;
        ss << "<a href=\"" << path << "\">" << it << "/</a>" << CHAR_LF;
        path += "/";
    }
}

static void 
__get_vector_of_path_nodes (
    std::string &parent, 
    std::vector<std::string> &path_nodes)
{
    std::string temp;
    //skip first character
    for (unsigned int index=1; index<parent.size(); ++index) {
        if (parent[index] == '/') {
            path_nodes.push_back (temp);
            temp.clear();
        }
        else {
            temp += parent[index];
        }
    }
}

static void __fill_src_addr (std::stringstream &ss, const std::string &src)
{
    ss << "Your IP address is " << src << "<br>" << CHAR_LF;
}

static 
const std::string 
_make_http_friendly (
    const std::string &filename)
{
    return std::regex_replace(filename, std::regex(" "), "%20");
}