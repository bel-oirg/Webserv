
#include "webserv.hpp"
#include "server.hpp"
#include "clients.hpp"
#include "parse.hpp"


using namespace std;

int	main(int argc, char **argv)
{
	if (argc == 2 && string(argv[1]) != "-h")
	{
		std::string		filename(argv[1]);
		std::vector<Server>	servers;
		ServersManager		manager;
		// size_t dot_pos = filename.rfind(".");

		// if ( dot_pos == string::npos || filename.substr(dot_pos) != "conf") ;;

			servers = Parse::get_servers(filename);


		{
			manager.init_servers(servers);
			// manager.print();
			cout << "\033[2J\033[H" << endl;
			manager.setup();
			manager.run();
		}
	}
	Parse::display_help();
}
