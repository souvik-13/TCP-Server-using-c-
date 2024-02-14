#include "http_tcpServer.hpp"
#include <iostream>

int main(int argc, char const *argv[])
{
    using namespace http;
    TcpServer server1 = TcpServer("0.0.0.0", 8080);
    // TcpServer server2 = TcpServer("0.0.0.0", 3000);
    server1.startListening();
    // server2.startListening();
    // std::string s;
    // while (std::cin >> s)
    // {
    //     if(s == "exit")
    //     {

    //         break;
    //     }
    //     else
    //     {
    //         std::cout << "Unknown command" << std::endl;
    //     }
    // }

    return 0;
}
