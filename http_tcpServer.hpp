#ifndef INCLUDED_HTTP_TCPSERVER_LINUX
#define INCLUDED_HTTP_TCPSERVER_LINUX

#include <string>       // This is the header file for the string class
#include <sys/socket.h> // This is the header file for the socket class
#include <arpa/inet.h>  // This is the header file for the internet operations
#include <unistd.h>     // This is the header file for the read and write operations
#include <stdlib.h>     // This is the header file for the exit function
#include <iostream>     // This is the header file for the standard input and output operations

namespace http
{
    class TcpServer
    {
    public:
        TcpServer(std::string ip_address, int port);
        void startListening();
        ~TcpServer();

    private:
        std::string m_ip_address;
        int m_port;
        int m_socket;
        int m_newSocket;
        long m_incomingMessage;
        struct sockaddr_in m_socketAddress;
        unsigned int m_socketAddressLength;
        std::string m_message;

        void setPort(int port);
        int startServer();
        void stopServer();
        void acceptConnection(int &new_socket);
        std::string buildResponse();
        void sendResponse();

        struct sockaddr_in
        {
            short sin_family;        // e.g. AF_INET, AF_INET6
            unsigned short sin_port; // e.g. htons(3490)
            struct in_addr sin_addr; // see struct in_addr, below
            char sin_zero[8];        // zero this if you want to
        };
        struct in_addr
        {
            unsigned long s_addr; // load with inet_pton()
        };
    };
} // namespace http
#endif