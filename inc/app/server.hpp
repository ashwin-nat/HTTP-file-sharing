#ifndef __SERVER_HPP__
#define __SERVER_HPP__

/**
 * @brief               - The infinite loop for the server
 * @param port          - port number to setup the server
 * @param tcp_backlog   - Max pending TCP connections
 * @param tpool_size    - Size of the thread pool
 */
void 
server_loop (
    int port,
    unsigned int tcp_backlog=5,
    unsigned int tpool_size=5);

#endif  //__SERVER_HPP__