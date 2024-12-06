
#include "webserv.hpp"
#include "server.hpp"
#include "clients.hpp"
// #include "config.hpp"
#include "parse.hpp"


using namespace std;

int	main(/*int argc, char **argv*/)
{
		std::string		filename("./test.conf");
		std::vector<Server>	servers;
		ServersManager		manager;

		servers = Parse::get_servers(filename);

		manager.init_servers(servers);
		manager.print();
		manager.setup();
		manager.run();
}
