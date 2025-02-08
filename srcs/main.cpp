
#include "webserv.hpp"
#include "server.hpp"
#include "clients.hpp"
#include "parse.hpp"

using namespace std;



int	main(int argc, char **argv)
{
	std::string		filename;

	switch (argc)
	{
		case 1:
			filename = "test.conf";
			break;
		case 2:
		{	
			if (string(argv[1]) != "-h")
			{
				filename = argv[1];
				break;
			}
		}
		default:
			Parse::display_help();
			break;
	}

	std::vector<Server>	servers;
	ServersManager		manager;

	servers = Parse::get_servers(filename);
	{
		manager.init_servers(servers);
		manager.print();
		// cout << "\033[2J\033[H" << endl;
		manager.setup();
		manager.run();
	}
}
