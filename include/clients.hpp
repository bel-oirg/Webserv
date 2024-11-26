
#include "server.hpp"
using namespace std;


class	PollFd
{
	private :
		std::vector<struct pollfd> fds;

	public :
	void add_fd(int fd)
	{
		struct pollfd neu;
		
		neu.fd = fd;
		neu.events = POLLIN;
		neu.revents = 0;

		fds.push_back(neu);
	}

	int get_fd(int index)
	{
		return (fds[index].fd);
	}

	struct pollfd* get_data()
	{
		return fds.data();
	}
	
	size_t size()
	{
		return fds.size();
	}

	void	log_info()
	{
		cout << "Size of Pool : " << fds.size();
		for (size_t i = 0; i < fds.size(); i++)
		{
			cout << "Fd number " << i << " : " << fds[i].fd <<  endl;
		}
		cout << "Done!!" << endl;
	}

};