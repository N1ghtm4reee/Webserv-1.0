#ifndef SERVERMANAGER_HPP
#define SERVERMANAGER_HPP

#include "Server.hpp"

class ServerManager
{
    private:
        ServerManager(){}
        std::vector<Server> _servers;
        std::vector<int> _sockets;
    public:
        ServerManager(std::vector<Server>& Servers)
        {
            _servers = Servers;
        }
        ~ServerManager(){}
        std::vector<int>& getSockets()
        {
            return _sockets;
        }
        void addServer(const Server& server)
        {
            _servers.push_back(server);
        }
        // void removeServer(int port)
        // {
        //     for (size_t i = 0; i < _servers.size(); ++i)
        //     {
        //         if (_servers[i].getPort() == port)
        //         {
        //             _servers.erase(_servers.begin() + i);
        //             break;
        //         }
        //     }
        // }
        void startServers()
        {
            for (size_t i = 0; i < _servers.size(); ++i)
            {
                // std::cout << "Location: " << _servers[i].getLocations()[0].getAllowedMethods()[0] << std::endl;
                std::vector<std::string> hosts = _servers[i].getHosts();
                std::vector<int> ports = _servers[i].getPorts();

                for (size_t h = 0; h < hosts.size(); ++h)
                {
                    for (size_t p = 0; p < ports.size(); ++p)
                    {
                        int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
                        if (serverSocket < 0)
                        {
                            std::cerr << "Error creating socket" << std::endl;
                            continue;
                        }
                        // int opt = 1;
                        // setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
                        struct sockaddr_in serverAddr;
                        serverAddr.sin_family = AF_INET;
                        serverAddr.sin_port = htons(ports[p]);

                        if (inet_pton(AF_INET, hosts[h].c_str(), &serverAddr.sin_addr) <= 0)
                        {
                            perror("Invalid address");
                            close(serverSocket);
                            continue;
                        }

                        if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
                        {
                            std::cerr << "Error binding socket to " << hosts[h] << ":" << ports[p] << std::endl;
                            close(serverSocket);
                            continue;
                        }

                        if (listen(serverSocket, 5) < 0)
                        {
                            std::cerr << "Error listening on socket " << hosts[h] << ":" << ports[p] << std::endl;
                            close(serverSocket);
                            continue;
                        }

                        _sockets.push_back(serverSocket);
                        std::cout << "Listening on " << hosts[h] << ":" << ports[p] << std::endl;
                    }
                }
            }
        }

        void stopServers()
        {
            for (size_t i = 0; i < _sockets.size(); ++i)
            {
                close(_sockets[i]);
            }
            _sockets.clear();
        }
        void handleRequest(int clientSocket);
        // void sendResponse(int clientSocket, const std::string& response);
        void closeConnection(int clientSocket)
        {
            close(clientSocket);
        }
        std::vector<Server>& getServers()
        {
            return _servers;
        }
};


#endif