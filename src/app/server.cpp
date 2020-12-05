#include "server.hpp"
#include "main.hpp"
#include "http.hpp"
#include "process_request.hpp"
#include "tpool.hpp"
#include "loguru.hpp"
#include <chrono>

static void _handle_connection (std::shared_ptr<TCPConnection> connection);
static std::string _format_time_double (const double &val);

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
    TPool tpool (tpool_size);

    while (1) {
        auto connection = server.accept_connection ();
        if (connection) {
            tpool.add_job (_handle_connection, connection);
        }
    }
}

static void 
_handle_connection (
    std::shared_ptr<TCPConnection> connection)
{
    std::vector<std::string> lines;
    HTTPRequest req;
    while (1) {
        const std::string src = connection->get_src_addr();
        req.clear();
        auto bytes = recv_req (connection, lines);
        if (bytes == HTTP_CLIENT_DISCONNECTED) {
            break;
        }
        else if (bytes == HTTP_READ_TIMEDOUT) {
            break;
        }

        auto start = std::chrono::high_resolution_clock::now();
        if (process_request(lines, connection) == false) {
            break;
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> time_taken = end-start;

        LOG_S(INFO) << "    " << "Request processed in " << 
            _format_time_double (time_taken.count()) << " seconds";

        break;
    }
}

static std::string 
_format_time_double (
    const double &val)
{
    char temp[15] = {0,};
    snprintf (temp, sizeof(temp)-1, "%.4f", val);
    std::string ret = temp;
    return ret;
}
