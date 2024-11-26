#include "config.hpp"
#include <sstream>
#include <map>




// struct pollfd Config::fds[1000]; 
// int Config::numfds = 0;         

// void	Config::accept_connections()
// {
// 	int cl = 0;
// 	struct pollfd tmp;
// 	for (;; )
// 	{
// 		int ret = accept(this->get_socket(), this->get_address(1), get_socklenp());
// 		if (ret < 0) {
//         if (errno == EAGAIN || errno == EWOULDBLOCK) {
// 			// std::clog << "no more clients" << endl;
//             break;
//         } else {
//             perror("accept failed");
//             exit(EXIT_FAILURE);
//         }
//     				}
// 		tmp.fd = ret;
// 		tmp.events = POLLIN;
// 		tmp.revents = 0; 
// 		Config::fds[Config::numfds] = tmp;
// 		Config::numfds++;
// 		// cout << "fd connection: "  << tmp.fd << endl;

// 		// char req[8192];
// 		// int r = read(ret, req, sizeof(req) - 1);
// 		// if (r > 0) 
// 		// 	req[r] = '\0';
// 		// cout << req << endl;
		
// 		cl++;
// 	}
// }


// void	Config::send_response(int index)
// {
// 	if (fds[index].events & POLLIN)
// 	{
// 		cout << "HERE" << endl; 
// 		write(fds[index].fd, http_response, strlen(http_response));
// 		close(fds[index].fd);
// 	}
// }







