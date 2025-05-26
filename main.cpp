#include "includes/Webserv.hpp"

ServerManager *g_serverManager = nullptr;

void handle_signal(int sig) {
    std::cout << "Shutting down..." << std::endl;
    // Close sockets, cleanup here
    if (g_serverManager) {
        g_serverManager->stopServers(); // You must implement this
    }

    exit(0);
}


int main(int ac, char **av)
{
    if (ac == 1 || ac == 2)
    {
        signal(SIGINT, handle_signal);
        signal(SIGTERM, handle_signal);
        std::string config_file = (ac == 2) ? av[1] : "webserv.conf";
        if (config_file.compare(config_file.size() - 5, 5, ".conf") != 0)
            return std::cerr << "Error: Config file must have a .conf extension" << std::endl, 1;
        ConfigParser parser;
        try{
            parser.parse(config_file);
            parser.printConfig();
            ServerManager serverManager(parser.getServers());
            g_serverManager = &serverManager;
            serverManager.startServers();
            // event loop to handle (connection and request and response actions with epoll)
            EventLoop EvenManager(serverManager);
            EvenManager.run();
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error: " << e.what() << std::endl;
            return 1;
        }
    }
    else
    {
        std::cerr << "Usage: " << av[0] << " [config_file]" << std::endl;
        return 1;
    }
}
