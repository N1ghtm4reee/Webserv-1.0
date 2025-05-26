#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "sockInfo.hpp"
#include "Request.hpp"
#include "Server.hpp"
#include <vector>
#include <map>
#include <string>
#include <poll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "Location.hpp"
#include <iostream>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <stdexcept>
#include <sstream>
#include <fcntl.h>
#include "Server.hpp"
#include <fstream>
#include <iostream>
#include <ctime>

class Response
{
    private:

    public:
        // std::string getResponseLine(int statusCode);
        // std::string readFile(const std::string &path);
        void sendErrorPage(int fd, Request &Req, int statusCode, Server &server);
        void sendResponse(int fd, Request &Req, int statusCode, Server &server);
        Location getLocation(std::string path, std::vector<Location> Locations);
        void sendPage(int fd, int statusCode, std::string path, std::string Connection);

    
};

#endif