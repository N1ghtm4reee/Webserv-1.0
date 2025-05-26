#include "includes/Webserv.hpp"

int main(int ac, char **av)
{
    if (ac == 1 || ac == 2)
    {
        std::string config_file = (ac == 2) ? av[1] : "webserv.conf";
        if (config_file.compare(config_file.size() - 5, 5, ".conf") != 0)
            return std::cerr << "Error: Config file must have a .conf extension" << std::endl, 1;
        ConfigParser parser;
        try{
            parser.parse(config_file);
            parser.printConfig();
            ServerManager serverManager(parser.getServers());
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
