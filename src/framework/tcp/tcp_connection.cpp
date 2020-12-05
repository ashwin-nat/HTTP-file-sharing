#include "tcp.hpp"
#include "prog_options.hpp"
#include "loguru.hpp"
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include <cstdio>
#include <sys/sendfile.h>
/******************************************************************************/
#define DEFAULT_BUFFER_SIZE     2048
/******************************************************************************/
extern ProgOptions prog_options;
/******************************************************************************/
class FILE_RAII {
private:
    FILE *m_fp;
public:
    explicit FILE_RAII (const std::string &filename) {
        m_fp = fopen (filename.c_str(), "rb");
    }
    ~FILE_RAII (void) {
        fclose (m_fp);
    }
    int get_fd (void) {
        if (m_fp) {
            return fileno (m_fp);
        }
        else {
            return -1;
        }
    }
};
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
    if (prog_options.verbose) {
        LOG_S(INFO) << "New TCP connection. fd = " << fd;
    }
}

/**
 * @brief       - Destroy the TCPConnection object
 */
TCPConnection :: ~TCPConnection (void)
{
    if (prog_options.verbose) {
        LOG_S(INFO) << "Destroying TCP connection with fd = " << m_fd;
    }
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
 * @brief           - Send the file through this TCP connection
 * @param filename  - name of the file to be sent
 * @param size      - number of bytes to be sent
 * @return ssize_t  - bytes sent
 */
ssize_t 
TCPConnection :: send (
    const std::string &filename,
    size_t size)
{
    FILE_RAII file (filename);
    int file_fd = file.get_fd ();
    if (file_fd == -1) {
        return file_fd;
    }

    off_t off=0;
    size_t total = 0;
    ssize_t n;
    size_t remaining = size;
    while (total < size) {
        //send out the data and check the return value
        n = sendfile (m_fd, file_fd, &off, remaining);
        if (n==-1) { return n; }

        total += n;
        off = total;
        remaining -= n;
    }
    return total;

    return 0;
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