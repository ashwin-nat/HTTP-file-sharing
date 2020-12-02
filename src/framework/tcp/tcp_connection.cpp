#include "tcp.hpp"
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
/******************************************************************************/
#define DEFAULT_BUFFER_SIZE     2048
/******************************************************************************/
static ssize_t _recv_wrapper (int fd, void *buffer, size_t size, int flags);
static ssize_t _send_wrapper (int fd, const char *buffer, size_t len,int flags);
/******************************************************************************/
/**
 * @brief       - Construct a new TCPConnection object
 * @param fd    - the fd for this connection
 * @param addr  - the client's source address
 */
TCPConnection :: TCPConnection (
    int fd,
    const sockaddr_in &addr)
{
    m_fd = fd;
    m_addr = addr;
    std::cout << "New TCP connection. fd = " << fd << std::endl;
}

/**
 * @brief       - Destroy the TCPConnection object
 */
TCPConnection :: ~TCPConnection (void)
{
    std::cout << "Destroying TCP connection with fd = " << m_fd << std::endl;
    close (m_fd);
    m_fd = -1;
    std::memset (&m_addr, 0, sizeof(m_addr));
}

/**
 * @brief       - Get the connection fd number
 * @return int  - the fd number
 */
int 
TCPConnection :: get_fd (void)
{
    return m_fd;
}
/**
 * @brief Get the name object
 * 
 * @return std::string 
 */
std::string 
TCPConnection :: get_name (void)
{
    return m_name;
}
/**
 * @brief       - Name this connection
 * @param name  - const reference to the string containing the name
 */

/**
 * @brief       - Name this connection
 * @param name  - const reference to the string containing the name
 */
void 
TCPConnection :: set_name (
    const std::string &name)
{
    m_name = name;
}

/**
 * @brief           - Receive data into given vector of uint8's
 * @param buffer    - reference to the vector of uint8's, clears the buffer 1st
 * @param flags     - flags for recv syscall
 * @return ssize_t  - bytes received if successful, -1 if failure, 0 if 
 *                      connection closed
 */
ssize_t 
TCPConnection :: recv (
    std::vector<char> &buffer,
    int flags)
{
    buffer.clear ();
    buffer.resize (DEFAULT_BUFFER_SIZE);
    return _recv_wrapper (m_fd, buffer.data(), buffer.size(), flags);
}

/**
 * @brief           - Send the data through this TCP connection
 * @param buffer    - vector containing the data
 * @param flags     - flags for send() system call
 * @return ssize_t  - number of bytes sent if success, -1 if failure
 */
ssize_t 
TCPConnection :: send (
    const std::vector<char> &buffer,
    int flags)
{
    return _send_wrapper (m_fd, buffer.data(), buffer.size(), flags);
}

/**
 * @brief           - Get the printable string version of the src address
 * @return std::string 
 */
std::string 
TCPConnection :: get_src_addr (void)
{
    char tmp_addr[INET_ADDRSTRLEN];

    inet_ntop (AF_INET, &(m_addr.sin_addr), tmp_addr, INET_ADDRSTRLEN);
    // std::string ret = tmp_addr;
    // return ret;
    return tmp_addr;
}
/******************************************************************************/
static ssize_t 
_recv_wrapper (
    int fd, 
    void *buffer, 
    size_t size, 
    int flags)
{
    return recv (fd, buffer, size, flags);
}

static ssize_t 
_send_wrapper (
    int fd, 
    const char *buffer, 
    size_t len, 
    int flags)
{
    size_t total = 0;
    ssize_t n;
    size_t remaining = len;
    while (total < len) {
        //send out the data and check the return value
        n = send (fd, buffer+total, remaining, flags);
        if (n==-1) { return n; }

        total += n;
        remaining -= n;
    }
    return total;
}