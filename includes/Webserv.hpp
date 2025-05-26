#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include "Server.hpp"
#include "Location.hpp"
#include "ConfigParser.hpp"
#include "utils.hpp"
#include "ServerManager.hpp"
#include "EventLoop.hpp"
#include "Request.hpp"
#include <iostream>
#include <vector>
#include <fstream>
#include <signal.h>


// class Webserv {
//     private:
//         std::string _config_file;
//         std::vector<Server> _servers;

//         // Private methods
//         void _parseConfig();
//         void _setupServers(){};

//     public:
//         // Constructors & Destructor
//         Webserv(const std::string &config_file);
//         ~Webserv() {};

//         // Public methods
//         void run(){};
// };


// bool containsWord(const std::string& str, const std::string& word);

#endif