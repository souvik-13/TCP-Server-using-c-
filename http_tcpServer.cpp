#include <iostream>
#include "http_tcpServer.hpp"
#include <sys/socket.h>
#include <unistd.h> // Include the necessary header file
#include <fstream>
#include <sstream> // Include the necessary header file
#include <cstring>

namespace
{
    /**
     * @brief This is the buffer size for the incoming message
     *
     * ? What is buffer?
     * => A buffer is a temporary storage location. It is used to store data when it is being moved from one place to another.
     * ? What is buffer overflow?
     * => A buffer overflow occurs when a program or process attempts to write more data to a fixed length block of memory, or buffer, than the buffer is allocated to hold.
     */
    const int BUFFER_SIZE = 30720;
    void writeToFile(const std::string message, std::string fileName)
    {
        std::string currentTime = "";
        time_t now = time(0);
        currentTime = ctime(&now);
        std::ofstream file;
        file.open(fileName, std::ios_base::app);
        // add new line character to the end of the message
        std::string logMessage = currentTime + " " + message + "\n";
        file << logMessage;
        file.close();
    }
    void log(const std::string &message, int line = __LINE__, std::string file = __FILE__)
    {
        std::ostringstream ss;
        ss << file << ":" << line << " " << message;
        std::cout << ss.str() << std::endl;
        writeToFile(ss.str(), "log.txt");
    }
    void exitWithError(const std::string &error_message, int line = __LINE__, std::string file = __FILE__)
    {
        std::ostringstream ss;
        ss << file << ":" << line << " " << error_message;
        std::cout << ss.str() << std::endl;
        std::cout << "detailed log is written to log.txt\n"
                  << std::endl;
        writeToFile(error_message, "log.txt");
        exit(1);
    }
}

namespace http
{
    /**
     * @brief Construct a new Tcp Server:: Tcp Server object
     * ? What does this constructor do?
     * => This constructor initializes the member variables of the class TcpServer. The member variables are:
     * 1. m_ip_address
     * 2. m_port
     * 3. m_socket
     * 4. m_newSocket
     * 5. m_incomingMessage
     * 6. m_socketAddress
     * 7. m_socketAddressLength
     * 8. m_message
     */
    TcpServer::TcpServer(std::string ip_address, int port)
        : m_ip_address(ip_address),
          m_port(port),
          m_socket(0),
          m_newSocket(0),
          m_incomingMessage(0),
          m_socketAddress(),
          m_socketAddressLength(sizeof(m_socketAddress)),
          m_message()
    {
        std::cout << "Calling the constructor." << std::endl;
        this->m_socketAddress.sin_family = AF_INET;
        this->m_socketAddress.sin_addr.s_addr = inet_addr(this->m_ip_address.c_str());
        this->m_socketAddress.sin_port = htons(this->m_port);
        if (startServer() != 0)
        {
            std::ostringstream ss;
            ss << "Failed to start server with PORT: " << ntohs(m_socketAddress.sin_port);
            log(ss.str(), __LINE__, __FILE__);
        }
    }

    /**
     * @brief Destroy the Tcp Server:: Tcp Server object
     * ? What does this destructor do?
     * => This destructor closes the socket connection.
     * ? When is this destructor called?
     * => This destructor is called when the object of the class TcpServer goes out of scope.
     */
    TcpServer::~TcpServer()
    {
        std::cout << "Calling the destructor." << std::endl;
        stopServer();
    }

    int TcpServer::startServer()
    {
        this->m_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (m_socket == -1)
        {
            exitWithError("Error: socket creation failed\n");
            return 1;
        }
        if (bind(
                this->m_socket,
                (struct sockaddr *)&this->m_socketAddress,
                sizeof(this->m_socketAddress)) < 0)
        {
            exitWithError("Error: socket binding failed\n");
            return 1;
        }
        return 0;
    }

    void TcpServer::startListening()
    {
        std::ostringstream ss;
        if (listen(this->m_socket, 20) < 0)
        {
            ss << "\n*** Server failed to accept incoming connection from "
               << this->m_ip_address << ":" << this->m_port << " ***\n";
            exitWithError("Error: socket listening failed\n");
        }
        ss << "\n*** Server started listening on " << this->m_ip_address << ":" << this->m_port << " ***\n";
        log(ss.str(), __LINE__, __FILE__);

        int bytesReceived;

        bool shouldBreakLoop = false;
        while (!shouldBreakLoop)
        {
            log("Waiting for incoming connection...", __LINE__, __FILE__);
            acceptConnection(this->m_newSocket);

            char buffer[BUFFER_SIZE] = {0};
            bytesReceived = read(this->m_newSocket, buffer, BUFFER_SIZE);
            if (bytesReceived < 0)
            {
                exitWithError("Error: socket reading failed\n");
            }

            ss << "\n*** Server received " << bytesReceived << " bytes from " << this->m_ip_address << ":" << this->m_port << " ***\n";
            log(ss.str(), __LINE__, __FILE__);

            this->m_message = buildResponse();
            sendResponse();

            close(this->m_newSocket);

            // Check for an external command to break the loop
            if (std::strcmp(buffer, "exit") == 0)
            {
                shouldBreakLoop = true;
            }
        }
    }

    void TcpServer::acceptConnection(int &new_socket)
    {
        new_socket = accept(
            this->m_socket,
            (struct sockaddr *)&this->m_socketAddress,
            (socklen_t *)&this->m_socketAddressLength);
        std::ostringstream ss;
        if (new_socket < 0)
        {
            exitWithError("Error: socket accepting failed\n");
            ss << "\n*** Server failed to accept incoming connection from " << this->m_ip_address << ":" << this->m_port << " ***\n";
        }
        ss << "\n*** Server accepted connection on " << this->m_ip_address << ":" << this->m_port << " ***\n";
        log(ss.str(), __LINE__, __FILE__);
    }

    std::string TcpServer::buildResponse()
    {
        std::string htmlFile = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>Document</title></head><body><h1>Hello from souvik's sever :)</h1></body></html>";
        std::ostringstream response;
        response << "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: " << std::to_string(htmlFile.length()) << "\n\n"
                 << htmlFile;
        return response.str();
    }

    void TcpServer::sendResponse()
    {
        long byteSent;
        std::ostringstream ss;
        byteSent = write(this->m_newSocket, this->m_message.c_str(), this->m_message.length());

        if (byteSent == m_message.size())
        {
            ss << "\n*** Server sent " << byteSent << " bytes to " << this->m_ip_address << ":" << this->m_port << " ***\n";
            log(ss.str(), __LINE__, __FILE__);
        }
        else
        {
            exitWithError("Error: socket writing failed\n");
        }
    }

    void TcpServer::stopServer()
    {
        close(this->m_socket);
        close(this->m_newSocket);
        std::cout << "Server stopped\n"
                  << std::endl;
        exit(0);
    }

    void TcpServer::setPort(int port)
    {
        this->m_port = port;
    }
} // namespace http