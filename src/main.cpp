#include <iostream>
#include "main.hpp"
#include "tcp.hpp"
#include "http.hpp"

void print_vec (const std::vector<char> &buffer)
{
    for (auto &it : buffer) {
        std::cout << it;
    }
}

void print_vec (const std::vector<std::string> &lines)
{
    for (auto &it : lines) {
        std::cout << it;
    }
}

int main (int argc, char *argv[])
{
    std::cout << "Hello world" << std::endl;

    // std::vector<char> buffer;
    // TCPServer server (5000);
    // while (1) {
    //     auto connection = server.accept_connection ();
    //     while (1) {
    //         auto bytes = connection->recv (buffer);
    //         if(bytes == 0) {
    //             std::cout << "connection closed" << std::endl;
    //             break;
    //         }
    //         print_vec (buffer);
            
    //         std::cout << "Received " << bytes << " bytes. ";
    //         std::cout << "sent " << connection->send(buffer) << " bytes" <<
    //                 std::endl;
    //     }

    HTTPServer server (5000);
    HTTPRequest req;
    HTTPResponse rsp;
    std::string hello = "Hello world";
    rsp.set_body (hello.c_str(), hello.size());
    rsp.m_status = HTTPStatus::HTTP_OK;
    rsp.m_content_type = "text/plain";

    std::vector<std::string> lines;
    while (1) {
        auto connection = server.accept_connection ();
        if (connection) {
            while (1) {
                std::string src = connection->get_src_addr();
                req.clear();
                auto bytes = server.recv_req (connection, lines);
                if (bytes == 0) {
                    break;
                }
                std::cout << "Received " << bytes << " bytes from " <<
                        src << " data = " << std::endl;
                print_vec (lines);


                req.parse (lines);
                bytes = server.send_rsp (connection, rsp);
                std::cout << "sent response of " << bytes << " bytes" << 
                        std::endl;
                // return 0;
            }
        }
    }


    return 0;
}