#ifndef SOCKINFO_HPP
#define SOCKINFO_HPP

#include <iostream>

class sockInfo
{
    private:
        int sockfd;
        int port;
        std::string host;
        int indexInServer;
    public:
        sockInfo(int sockfd, int port, std::string host, int indexInServer)
            : sockfd(sockfd), port(port), host(host), indexInServer(indexInServer) {}
        ~sockInfo() {}
        int getSockfd() { return sockfd; }
        int getPort() { return port; }
        std::string getHost() { return host; }
        int getIndexInServer() { return indexInServer; }

        void print()
        {
            std::cout << "sockfd: " << sockfd << ", port: " << port << ", host: " << host << ", indexInServer: " << indexInServer << std::endl;
        }
};

#endif