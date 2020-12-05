#include "tcp.hpp"
#include "prog_options.hpp"
#include <unistd.h>
#include <arpa/inet.h>
#include <stdexcept>
#include <cstring>
#include <cerrno>
/******************************************************************************/
extern ProgOptions prog_options;
/******************************************************************************/
static void _raise_exception (const std::string &msg);
/******************************************************************************/
/**
 * @brief       - Construct a new TCPServer object
 * @param port  - port number to bind to
 * @param backlog-the tcp connection backlog
 */
TCPServer :: TCPServer (
    int port,
    int backlog)
{
    //create the socket
    m_fd = socket (AF_INET, SOCK_STREAM, 0);
    if (m_fd == -1) {
        //raise exception here
    }

    //set SO_REUSEADDR
    int opt = 1;
    if(setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        close(m_fd);
        _raise_exception ("setsockopt failed");
    }

    //bind to the local ipv4 addr and given port
    struct sockaddr_in addr = {0,};
    addr.sin_family     = AF_INET;
    addr.sin_addr.s_addr= htonl (INADDR_ANY);
    addr.sin_port       = htons (port);

    if(bind(m_fd, reinterpret_cast<sockaddr*> (&addr), sizeof(addr)) == -1) {
        close(m_fd);
        _raise_exception("bind failed");
    }

    //setup connection backlog
    if(listen(m_fd, backlog) == -1) {
        close(m_fd);
        _raise_exception("listen failed");
    }

    m_port = port;
    if (prog_options.verbose) {
        std::cout << "Created TCP server at port " << m_port 
            << ". fd = " << m_fd << std::endl;
    }
}

/**
 * @brief       - Destroy the TCPServer object
 */
TCPServer :: ~TCPServer (void)
{
    if (prog_options.verbose) {
        std::cout << "Destroying TCP server with fd = " << m_fd << std::endl;
    }
    close (m_fd);
    m_port = 0;
    m_fd = -1;
    m_name.clear();
}

/**
 * @brief       - Get the server fd
 * @return int  - fd number
 */
int 
TCPServer :: get_fd (void)
{
    return m_fd;
}
/**
 * @brief       - Get the server object name
 * @return std::string - name string
 */
std::string 
TCPServer :: get_name (void)
{
    return m_name;
}

/**
 * @brief       - Set the socket name string, for debugging
 * @param name  - const reference to the string containing the name
 */
void 
TCPServer :: set_name (
    const std::string &name)
{
    m_name = name;
}

/**
 * @brief       - Accepts a new connection and returns a unique ptr to the 
 *                  connection object
 * @return std::unique_ptr<TCPConnection> - ptr to connection object
 */
std::shared_ptr<TCPConnection> 
TCPServer :: accept_connection (void)
{
    sockaddr_in src = {0,};
    socklen_t srclen = sizeof(src);
    int fd = accept (m_fd, reinterpret_cast<sockaddr*> (&src), &srclen);

    //return nullptr is accept fails for whatever reason
    if (fd == -1) {
        return nullptr;
    }
    
    auto ret = std::make_shared <TCPConnection>(fd, src);
    return ret;
}

static void 
_raise_exception (
    const std::string &msg)
{
    throw std::invalid_argument (msg + ". " + strerror(errno));
}