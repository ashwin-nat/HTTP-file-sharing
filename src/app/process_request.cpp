#include "process_request.hpp"
#include "http.hpp"
#include "filesys.hpp"
#include "loguru.hpp"
#include "prog_options.hpp"
#include "blacklist.hpp"
#include <sstream>

extern ProgOptions prog_options;

static void process_get_request (HTTPRequest &req, HTTPResponse &rsp,
    const std::string &src);

HTTPStatus build_rsp_html (std::vector<char> &buffer, HTTPRequest &req,
    std::vector <FSEntry> &tree, const std::string &src);
void get_404_page (HTTPRequest &req, std::vector<char> &buffer,
    const std::string &src);
HTTPStatus get_file (HTTPResponse &rsp, HTTPRequest &req);
bool check_if_file (const std::string &filename);

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
    const std::string fail = "Unsupported HTTP method";
    HTTPResponse rsp;
    rsp.m_content_type = "text/plain";

    ssize_t bytes;
    if(req.m_method == "GET") {
	    std::stringstream ss;
        ss << "Received request from " << src << ": ";
        ss << "GET " << req.m_uri << " ";

        auto result = req.m_header.find ("User-Agent");
        if (result != req.m_header.end()) {
            ss << result->first << ": " <<
                result->second;
        }
        LOG_S(INFO) << ss.str();
        process_get_request (req, rsp, src);
    }
    else {
        rsp.set_body (fail.c_str(), fail.size());
        rsp.m_status = HTTPStatus::HTTP_UNSUPP_METHOD;

        LOG_S(WARNING) << "Received " << req.m_method << " request from" << src;
    }

    bytes = send_http_rsp (connection, rsp);
    if (bytes > 0 ) {
        int score=0;
        LOG_S(INFO) << "Sent response of " << bytes_human_readable (bytes) <<
            " to " << src << ". Status = " << http::to_string (rsp.m_status) <<
            ((get_score_for_ipaddr(src, score)==true) ?
            std::string (" with score=" + std::to_string(score)) :
            std::string ("") );
    }
    if (prog_options.verbose) {
        LOG_S(INFO) << "Content-Type: " << rsp.m_content_type;
    }
    const bool good_request = (rsp.m_status == HTTPStatus::HTTP_OK) ?
        (true) : (false);;
    return update_ip_addr_stats (src, good_request);
}

static void
process_get_request (
    HTTPRequest &req,
    HTTPResponse &rsp,
    const std::string &src)
{
    std::vector<char> rspbuff;
    //if request is a file, serve the file
    if (check_if_file (req.m_uri)) {

        rsp.m_status = get_file (rsp, req);
        if (rsp.m_status == HTTPStatus::HTTP_NOT_FOUND) {
            get_404_page (req, rspbuff, src);
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
                get_404_page (req, rspbuff, src);
            }
            else {
                rsp.m_status = HTTPStatus::HTTP_GEN_ERR;
            }
        }
        else {
            rsp.m_status = build_rsp_html (rspbuff, req, tree, src);
        }
        if (rspbuff.size() > 0) {
            rsp.set_body (rspbuff.data(), rspbuff.size());
            rsp.m_content_type = "text/html; charset=utf-8";
        }
    }


}

