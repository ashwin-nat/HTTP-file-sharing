#include "server.hpp"
#include "main.hpp"
#include "http.hpp"
#include "process_request.hpp"

/**
 * @brief               - The infinite loop for the server
 * @param port          - port number to setup the server
 * @param tcp_backlog   - Max pending TCP connections
 * @param tpool_size    - Size of the thread pool
 */
void 
server_loop (
    int port,
    unsigned int tcp_backlog,
    unsigned int tpool_size)
{
    HTTPServer server (port, tcp_backlog);
    HTTPRequest req;
    HTTPResponse rsp;
    std::string hello = "Hello world";
    rsp.set_body (hello.c_str(), hello.size());
    rsp.m_status = HTTPStatus::HTTP_OK;
    rsp.m_content_type = "text/plain";

    HTTPResponse rsp_fail;
    std::string fail = "Unsupported HTTP method";
    rsp_fail.m_status = HTTPStatus::HTTP_UNSUPP_METHOD;
    rsp_fail.m_content_type = "text/plain";
    rsp_fail.set_body (fail.c_str(), fail.size());

    std::vector<std::string> lines;
    while (1) {
        auto connection = server.accept_connection ();
        if (connection) {
            while (1) {
                const std::string src = connection->get_src_addr();
                req.clear();
                auto bytes = server.recv_req (connection, lines);
                if (bytes == HTTP_CLIENT_DISCONNECTED) {
                    break;
                }
                else if (bytes == HTTP_READ_TIMEDOUT) {
                    break;
                }
                if (process_request(lines, connection) == false) {
                    break;
                }
            }
        }
    }
}