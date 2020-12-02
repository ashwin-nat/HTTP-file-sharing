/**
 * @author          - Ashwin Natarajan
 * @brief           - A simple wrapper class for sockets and fd management
 * @date 2020-11-26
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef __SOCKET_HPP__
#define __SOCKET_HPP__

#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <memory>
#include <vector>
#include <iostream>

class TCPConnection {
private:
    int m_fd;
    sockaddr_in m_addr;
    std::string m_name;
public:
    /**
     * @brief       - Construct a new TCPConnection object
     * @param fd    - the fd for this connection
     * @param addr  - the client's source address
     */
    TCPConnection (
        int fd,
        const sockaddr_in &addr);
    /**
     * @brief       - Destroy the TCPConnection object
     */
    ~TCPConnection (void);
    /**
     * @brief       - Get the connection fd number
     * @return int  - the fd number
     */
    int 
    get_fd (void);
    /**
     * @brief Get the name object
     * 
     * @return std::string 
     */
    std::string 
    get_name (void);
    /**
     * @brief       - Name this connection
     * @param name  - const reference to the string containing the name
     */
    void 
    set_name (
        const std::string &name);
    /**
     * @brief           - Receive data into given vector of uint8's
     * @param buffer    - reference to the vector of uint8's
     * @param flags     - flags for recv syscall
     * @return ssize_t  - bytes received if successful, -1 if failure, 0 if 
     *                      connection closed
     */
    ssize_t 
    recv (
        std::vector<char> &buffer,
        int flags = 0);
    /**
     * @brief           - Send the data through this TCP connection
     * @param buffer    - vector containing the data
     * @param flags     - flags for send() system call
     * @return ssize_t  - number of bytes sent if success, -1 if failure
     */
    ssize_t 
    send (
        const std::vector<char> &buffer,
        int flags=0);
    /**
     * @brief           - Get the printable string version of the src address
     * @return std::string 
     */
    std::string 
    get_src_addr (void);
};

class TCPServer {
private:
    int m_port;
    int m_fd;
    std::string m_name;
public:
    /**
     * @brief       - Construct a new TCPServer object
     * @param port  - port number to bind to
     * @param backlog-the tcp connection backlog
     */
    explicit TCPServer (
        int port,
        int backlog=5);
    /**
     * @brief       - Destroy the TCPServer object
     */
    ~TCPServer (void);
    /**
     * @brief       - Get the server fd
     * @return int  - fd number
     */
    int 
    get_fd (void);
    /**
     * @brief       - Get the server object name
     * @return std::string - name string
     */
    std::string 
    get_name (void);
    /**
     * @brief       - Set the socket name string, for debugging
     * @param name  - const reference to the string containing the name
     */
    void 
    set_name (
        const std::string &name);
    /**
     * @brief       - Accepts a new connection and returns a unique ptr to the 
     *                  connection object
     * @return std::unique_ptr<TCPConnection> - ptr to connection object
     */
    std::unique_ptr<TCPConnection> 
    accept_connection (void);
};

#endif  //__SOCKET_HPP__