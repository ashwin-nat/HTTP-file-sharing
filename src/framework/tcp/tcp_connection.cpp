#include "tcp.hpp"
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
/******************************************************************************/

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
    std::cout << "New connection. fd = " << fd << std::endl;
}

/**
 * @brief       - Destroy the TCPConnection object
 */
TCPConnection :: ~TCPConnection (void)
{
    std::cout << "Destroying connection with fd = " << m_fd << std::endl;
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