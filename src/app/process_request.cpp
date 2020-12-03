#include "process_request.hpp"
#include "http.hpp"
#include "filesys.hpp"

static void process_get_request (HTTPRequest &req, HTTPResponse &rsp);
    
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
    std::cout << "Received request from " << src << std::endl;
    HTTPResponse rsp;
    rsp.m_content_type = "text/plain";

    ssize_t bytes;
    if(req.m_method == "GET") {
        std::cout << "\tGET " << req.m_uri << std::endl;
        process_get_request (req, rsp);
    }
    else {
        rsp.set_body (fail.c_str(), fail.size());
        rsp.m_status = HTTPStatus::HTTP_UNSUPP_METHOD;
    }
    bytes = send_http_rsp (connection, rsp);
    std::cout << "Sent " << bytes << " to " << src << std::endl;
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
        // const std::string file = "Coming soon....";
        // rsp.m_status = HTTPStatus::HTTP_OK;
        // rsp.set_body (file.c_str(), file.size());

        rsp.m_status = get_file (rsp, req);
        if (rsp.m_status == HTTPStatus::HTTP_NOT_FOUND) {
            get_404_page (req, rspbuff);
            rsp.set_body (rspbuff.data(), rspbuff.size());
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