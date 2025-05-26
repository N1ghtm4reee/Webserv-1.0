#ifndef SERVER_HPP
#define SERVER_HPP

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

class Location;

// Server class to store server block configuration
class Server {
private:
    std::vector<int> _ports;
    std::vector<std::string> _hosts;
    std::vector<std::string> _serverNames;
    size_t _clientMaxBodySize;
    std::map<int, std::string> _errorPages;
    std::vector<Location> _locations;

public:
    Server() : _clientMaxBodySize(1024 * 1024) {
        _hosts.push_back("0.0.0.0");
        _ports.push_back(80);
    } // Default values
    
    void setHost(const std::string& host) { _hosts.push_back(host); }
    void setPort(int port)
    {
        if (port > 65535 || port < 1)
            throw std::runtime_error("Error: Invalid port number: " + std::to_string(port));
        _ports.push_back(port);
    }
    void setServerName(const std::string& name) { _serverNames.push_back(name); }
    void setClientMaxBodySize(size_t size) { _clientMaxBodySize = size; }
    void addErrorPage(int code, const std::string& path) { _errorPages[code] = path; }
    void addLocation(const Location& location) { _locations.push_back(location); }
    
    // Getters
    std::vector<std::string> getHosts() { return _hosts; }
    std::vector<int> getPorts() const { return _ports; }
    std::vector<std::string>& getServerNames() { return _serverNames; }
    size_t getClientMaxBodySize() const { return _clientMaxBodySize; }
    const std::map<int, std::string>& getErrorPages() const { return _errorPages; }
    const std::vector<Location>& getLocations() const { return _locations; }
    
    void removePort(std::string port)
    {
        for (int i = 0; i < _ports.size(); i++)
        {
            if (_ports[i] == std::stoi(port))
                _ports.erase(_ports.begin() + i);
        }
    }
    void removeHost(std::string host)
    {
        for (int i = 0; i < _hosts.size(); i++)
        {
            if (_hosts[i] == host)
                _hosts.erase(_hosts.begin() + i);
        }
    }

    // Debug print
    void print() const {
        std::cout << "Server:" << std::endl;
        for(auto& host : _hosts)
            std::cout << "  Host: " << host << std::endl;
        for(int i = 0; i < _ports.size(); i++)
            std::cout << "  Port: " << _ports[i] << std::endl;
        for(const auto& name : _serverNames)
            std::cout << "  Server Name: " << name << std::endl;
        std::cout << "  Max Body Size: " << _clientMaxBodySize << std::endl;
        
        if (!_errorPages.empty()) {
            std::cout << "  Error Pages:" << std::endl;
            for (const auto& page : _errorPages) {
                std::cout << "    " << page.first << " -> " << page.second << std::endl;
            }
        }
        
        if (!_locations.empty()) {
            std::cout << "  Locations:" << std::endl;
            for (const auto& loc : _locations) {
                loc.print();
            }
        }
    }
};

// class Server {
//     private:
//         int _port;
//         std::string _host;
//         std::string _server_name;
//         size_t _client_max_body_size;
//         std::map<int, std::string> _error_pages;
//         std::vector<Location> _locations;
//         int _socket_fd;
//         struct sockaddr_in _socket_addr;

//         // Private methods
//         void _setupSocket();
//         void _bindSocket();
//         void _listenSocket();

//     public:
//         // Constructors & Destructor
//         Server();
//         Server(const Server &other);
//         ~Server();
//         Server &operator=(const Server &other);

//         // Configuration methods
//         void setPort(int port);
//         void setHost(const std::string &host);
//         void setServerName(const std::string &name);
//         void setClientMaxBodySize(size_t size);
//         void addErrorPage(int error_code, const std::string &path);
//         void addLocation(const Location &location);

//         // Getters
//         int getPort() const;
//         const std::string &getHost() const;
//         const std::string &getServerName() const;
//         size_t getClientMaxBodySize() const;
//         const std::string &getErrorPage(int error_code) const;
//         const std::vector<Location> &getLocations() const;
//         int getSocketFd() const;

//         // Server operations
//         void setup();
//         void closeSocket();
// };

#endif