#include "http.hpp"
#include <poll.h>
#include <assert.h>
/******************************************************************************/
#define CHAR_CR         '\r'
#define CHAR_LF         '\n'
/******************************************************************************/

/******************************************************************************/
/**
 * @brief       - Construct a new HTTP server object
 * @param port  - Port number to bind to
 * @param backlog-TCP connection backlog
 */
HTTPServer :: HTTPServer (
    int port,
    int backlog)
{
    m_tcp_server = std::make_unique <TCPServer> (port);
}

/**
 * @brief       - Accept an incoming connection request
 * @return std::shared_ptr<TCPConnection> - ptr to the connection object
 */
std::shared_ptr<TCPConnection> 
HTTPServer :: accept_connection (void)
{
    return m_tcp_server->accept_connection ();
}

/**
 * @brief           - Receive a HTTP request
 * @param client    - shared ptr to client connection object
 * @param buffer    - vector of strings where the req is to be stored
 * @param timeout   - The timeout for recv (in milliseconds) 
 *                      (0 implies no timeout)
 * @return ssize_t  - number of bytes received, or -1 if err, or 0 if 
 *                      client disconnected, -2 if timedout
 */
ssize_t 
recv_req (
    std::shared_ptr<TCPConnection> client,
    std::vector<std::string> &buffer,
    unsigned int timeout)
{
    ssize_t ret=0;
    std::vector<char> tmp_buffer;
    if (timeout==0) {
        ret = client->recv (tmp_buffer);
    }

    //use poll to timeout
    else {
        pollfd pfd = {.fd = client->get_fd(), .events = POLLIN};
        switch (poll (&pfd, 1, timeout)) {
        //timedout
        case 0:
            ret = HTTP_READ_TIMEDOUT;
            break;
        //syscall error
        case -1:
            ret = -1;
            break;
        //fd ready for recv
        case 1:
            ret = client->recv (tmp_buffer);
            break;
        }
    }

    //build the vector of strings if data was received
    if (ret > 0) {
        std::string temp_str;
        buffer.clear();
        bool new_line_begin=false;
        for (auto &it : tmp_buffer) {
            //if new_line_begin is true, then we finish the old line and start
            //new line
            if (new_line_begin) {
                new_line_begin = false;
                buffer.push_back (temp_str);
                temp_str.clear();
            }

            //add upto LF to this line (LF should be part of the line)
            if (it == CHAR_LF) {
                new_line_begin = true;
            }
            temp_str += it;
        }
    }
    return ret;
}

/**
 * @brief           - Send HTTP Response to the client
 * @param client    - pointer to the connection object
 * @param rsp       - reference to the HTTPResponse struct
 * @return ssize_t  - number of bytes sent if successful, else -1
 */
ssize_t 
send_http_rsp (
    std::shared_ptr<TCPConnection> client,
    HTTPResponse &rsp)
{
    ssize_t ret;
    std::vector <char> rspbuff;
    rsp.build_rsp (rspbuff);
    assert (rsp.m_status != HTTPStatus::HTTP_GEN_ERR);
    ret = client->send (rspbuff);

    if (!rsp.m_rsp_filename.empty()) {
        //send the file as well
        ret += client->send (rsp.m_rsp_filename, rsp.m_len);
    }
    return ret;
}