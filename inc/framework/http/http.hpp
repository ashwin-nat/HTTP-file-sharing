#ifndef __HTTP_HPP__
#define __HTTP_HPP__

#include "tcp.hpp"
#include "http_req.hpp"
#include "http_rsp.hpp"

#define SERVER_HTTP_VER                 "1.0"
#define HTTP_READ_TIMEDOUT              -2
#define HTTP_CLIENT_DISCONNECTED        0
#define HTTP_READ_INFINITE_TIMEOUT      0
#define CHAR_SPACE                      ' '
#define CHAR_COLON                      ':'
#define CHAR_CR                         '\r'
#define CHAR_LF                         '\n'
#define SERVER_INFO                     "Ashwin HTTP Server"

class HTTPServer{
private:
    std::shared_ptr<TCPServer> m_tcp_server;
public:
    /**
     * @brief       - Construct a new HTTP server object
     * @param port  - Port number to bind to
     * @param backlog-TCP backlog
     */
    explicit 
    HTTPServer (
        int port,
        int backlog=5);
    /**
     * @brief       - Accept an incoming connection request
     * @return std::shared_ptr<TCPConnection> - ptr to the connection object
     */
    std::shared_ptr<TCPConnection> 
    accept_connection (void);
    /**
     * @brief           - Receive a HTTP request
     * @param client    - unique ptr to client connection object
     * @param buffer    - vector of strings where the req is to be stored
     * @param timeout   - the timeout for recv  (in milliseconds)
     *                          (0 implies no timeout)
     * @return ssize_t  - number of bytes received, or -1 if failed, or 0 if 
     *                      client disconnected, -2 if timedout
     */
    ssize_t 
    recv_req (
        std::shared_ptr<TCPConnection> client,
        std::vector<std::string> &buffer,
        unsigned int timeout=0);
};

/**
 * @brief           - Send HTTP Response to the client
 * @param client    - pointer to the connection object
 * @param rsp       - reference to the HTTPResponse struct
 * @return ssize_t  - number of bytes sent if successful, else -1
 */
ssize_t 
send_http_rsp (
    std::shared_ptr<TCPConnection> client,
    HTTPResponse &rsp);

#endif  //__HTTP_HPP__