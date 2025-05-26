#include "EventLoop.hpp"

EventLoop::EventLoop(ServerManager& ServerManager) : _ServerManager(ServerManager)
{
    server_fds = _ServerManager.getSockets();
    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1)
    {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }
    for (size_t i = 0; i < server_fds.size(); ++i)
    {
        addToEpoll(server_fds[i], EPOLLIN);
    }
}

int EventLoop::findServerIndex(std::string host, std::string port, std::vector<Server> servers)
{
    int targetPort;
    try {
        targetPort = std::stoi(port);
    } catch (const std::exception& e) {
        return -1;
    }
    
    for (size_t i = 0; i < servers.size(); i++)
    {
        std::vector<std::string> hosts = servers[i].getHosts();
        std::vector<int> ports = servers[i].getPorts();
        
        bool hostMatch = false;
        for (size_t j = 0; j < hosts.size(); j++)
        {
            if (hosts[j] == host)
            {
                hostMatch = true;
                break;
            }
        }
        
        bool portMatch = false;
        for (size_t k = 0; k < ports.size(); k++)
        {
            if (ports[k] == targetPort)
            {
                portMatch = true;
                break;
            }
        }
        
        if (hostMatch && portMatch)
            return static_cast<int>(i);
    }
    return -1;
}

void EventLoop::addToEpoll(int fd, int mode)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
    {
        perror("fcntl F_GETFL");
        close(fd);
        exit(EXIT_FAILURE);
    }

    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
    {
        perror("fcntl F_SETFL");
        close(fd);
        exit(EXIT_FAILURE);
    }

    struct epoll_event ev;
    ev.events = mode;
    ev.data.fd = fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev) == -1)
    {
        perror("epoll_ctl: listen_sock");
        close(fd);
        exit(EXIT_FAILURE);
    }
}

void EventLoop::modifyEpollEvent(int fd, int mode)
{
    struct epoll_event ev;
    ev.events = mode;
    ev.data.fd = fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &ev) == -1)
    {
        perror("epoll_ctl: listen_sock");
        close(fd);
        exit(EXIT_FAILURE);
    }
}

int EventLoop::isServerSocket(int fd)
{
    for (int i = 0; i < server_fds.size(); i++)
    {
        if (fd == server_fds[i])
            return i + 1;
    }
    return 0;
}

void EventLoop::parseRequest(int fd)
{
    char buffer[1025];
    ssize_t bytes = recv(fd, buffer, 1024, 0);

    if (bytes <= 0)
    {
        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, nullptr);
        close(fd);
        requests.erase(fd);
        responses.erase(fd);
        statusCodes.erase(fd);
    }
    else
    {
        buffer[bytes] = '\0';
        buffers[fd] += std::string(buffer, bytes);

        if (requests[fd].isComplete(buffers[fd]))
        {
            int statusCode = requests[fd].parse(buffers[fd]);
            int sp = requests[fd].getHeader("Host").find(":");
            socketInfo.insert(std::make_pair(fd, sockInfo(fd, std::stoi(requests[fd].getHeader("Host").substr(sp + 1)), requests[fd].getHeader("Host").substr(0, sp), findServerIndex(requests[fd].getHeader("Host").substr(0, sp), requests[fd].getHeader("Host").substr(sp + 1), _ServerManager.getServers()))));
            std::map<int, sockInfo>::iterator it = socketInfo.find(fd);
            if (it != socketInfo.end())
                it->second.print();
            statusCodes[fd] = statusCode;
            modifyEpollEvent(fd, EPOLLOUT);
        }
    }
}

void EventLoop::acceptNewConnection(int fd)
{
    sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_fd = accept(fd, (sockaddr *)&client_addr, &client_len);
    if (client_fd < 0)
        std::cerr << "error in client_fd\n";
    addToEpoll(client_fd, EPOLLIN);
    std::cout << "Accepted new client on fd " << client_fd << "\n";
}

void EventLoop::run()
{
    while (true)
    {
        struct epoll_event events[MAX_EVENTS];
        int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, TIMEOUT);
        if (nfds == -1)
        {
            perror("epoll_wait");
            exit(EXIT_FAILURE);
        }

        for (int n = 0; n < nfds; ++n)
        {
            int fd = events[n].data.fd;

            if (events[n].events & EPOLLIN)
            {
                int IndexServ = 0;
                if ((IndexServ = isServerSocket(fd)))
                    acceptNewConnection(fd);
                else
                    parseRequest(fd);
            }
            else if (events[n].events & EPOLLOUT)
            {
                responses[fd].sendResponse(fd, requests[fd], statusCodes[fd], _ServerManager.getServers()[socketInfo.find(fd)->second.getIndexInServer()]);

                std::string conn = requests[fd].getHeader("Connection");
                if (conn == "close" || conn == "Close")
                {
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, nullptr);
                    close(fd);
                    requests.erase(fd);
                    responses.erase(fd);
                    statusCodes.erase(fd);
                }
                else
                    modifyEpollEvent(fd, EPOLLIN);
                resetClientdata(fd);
            }
        }
    }
}
