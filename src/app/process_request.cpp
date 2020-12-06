#include "process_request.hpp"
#include "http.hpp"
#include "filesys.hpp"
#include "loguru.hpp"
#include "prog_options.hpp"
#include <sstream>

#define _LAST_N_BITS_SET(n)             ((2 << ((n)-1))-1)

extern ProgOptions prog_options;

static void process_get_request (HTTPRequest &req, HTTPResponse &rsp);
static std::string _bytes_human_readable (size_t bytes);
    
HTTPStatus build_rsp_html (std::vector<char> &buffer, HTTPRequest &req, 
    std::vector <FSEntry> &tree);
void get_404_page (HTTPRequest &req, std::vector<char> &buffer);
HTTPStatus get_file (HTTPResponse &rsp, HTTPRequest &req);

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
    std::shared_ptr<TCPConnection> connection)
{
    HTTPRequest req;
    if (req.parse(lines) == false) {
        return false;
    }

    const std::string src = connection->get_src_addr();
    // const std::string hello = "Hello world";
    const std::string fail = "Unsupported HTTP method";
    HTTPResponse rsp;
    rsp.m_content_type = "text/plain";

    ssize_t bytes;
    if(req.m_method == "GET") {
	std::stringstream ss;
        ss << "Received request from " << src << ": ";
        ss << "GET " << req.m_uri;

	auto result = req.m_header.find ("User-Agent");
	if (result != req.m_header.end()) {
		ss << result->first << ": " << 
			result->second;
	}
	LOG_S(INFO) << ss.str();
        process_get_request (req, rsp);
    }
    else {
        rsp.set_body (fail.c_str(), fail.size());
        rsp.m_status = HTTPStatus::HTTP_UNSUPP_METHOD;
    }
    bytes = send_http_rsp (connection, rsp);
    LOG_S(INFO) << "Sent response of " << _bytes_human_readable (bytes) << 
        " to " << src;
    if (prog_options.verbose) {
        LOG_S(INFO) << "Content-Type: " << rsp.m_content_type;
    }
    return true;
}

static void 
process_get_request (
    HTTPRequest &req,
    HTTPResponse &rsp)
{
    std::vector<char> rspbuff;
    //if request is a file, serve the file
    if (is_file (req.m_uri)) {

        rsp.m_status = get_file (rsp, req);
        if (rsp.m_status == HTTPStatus::HTTP_NOT_FOUND) {
            get_404_page (req, rspbuff);
            rsp.set_body (rspbuff.data(), rspbuff.size());
            rsp.m_content_type = "text/html; charset=utf-8";
        }
    }
    else {
        std::vector <FSEntry> tree;
        //else serve the response HTML page
        if (get_file_listing (req.m_uri, tree) == false) {
            if (errno == ENOENT) {
                rsp.m_status = HTTPStatus::HTTP_NOT_FOUND;
                get_404_page (req, rspbuff);
            }
            else {
                rsp.m_status = HTTPStatus::HTTP_GEN_ERR;
            }
        }
        else {
            rsp.m_status = build_rsp_html (rspbuff, req, tree);
        }
        if (rspbuff.size() > 0) {
            rsp.set_body (rspbuff.data(), rspbuff.size());
            rsp.m_content_type = "text/html; charset=utf-8";
        }
    }
}

static std::string 
_bytes_human_readable (
    size_t bytes)
{
    static const std::vector<std::string> sizes = { "B", "kB", "MB", "GB", "TB"};
    size_t order = 0;
    size_t quot=bytes, rem=0;

    while (quot > 1024) {
        //bit shift for fun, basically rem = quot%1024
        rem = quot & (_LAST_N_BITS_SET(10));
        //bit shift for fun, basically quot = quot/1024
        quot = quot >> 10;
        order++;
    }

    std::string ret = std::to_string (quot) + "." + std::to_string (rem);
    if (order >= sizes.size()) {
        order = sizes.size()-1;
    }

    ret += " " + sizes[order];
    return ret;
}
