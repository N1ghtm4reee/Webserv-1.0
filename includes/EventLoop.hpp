#ifndef EVENTLOOP_HPP
#define EVENTLOOP_HPP

#define MAX_EVENTS 10
#define TIMEOUT 1000

#include <iostream>
#include <vector>
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <stdexcept>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string>
#include <sstream>
#include <unordered_set>
#include <cstdlib>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include "ServerManager.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "sockInfo.hpp"

class EventLoop
{
    private:
        EventLoop() = delete;
        ServerManager _ServerManager;
        int epoll_fd;
        std::vector<int> server_fds;
        // because each request overwrite the previous one
        std::map<int, Request> requests;
        // because each response overwrite the previous one
        std::map<int, Response> responses;
        // std::vector<int> client_fds;
        // std::vector<int> ready_fds;
        std::map<int, int> statusCodes;
        std::map<int, std::string> buffers;
        std::map<int, sockInfo> socketInfo;

    public:
        void resetClientdata(int fd)
        {
            requests.erase(fd);
            responses.erase(fd);
            buffers.erase(fd);
            statusCodes[fd] = 0;
        }

        EventLoop(ServerManager& ServerManager);
        ServerManager getServerManager() { return _ServerManager; }
        void addToEpoll(int fd, int mode);
        void modifyEpollEvent(int fd, int mode);
        int isServerSocket(int fd);
        int findServerIndex(std::string host, std::string port, std::vector<Server> servers);
        void parseRequest(int fd);
        void acceptNewConnection(int fd);
        void run();
};

#endif