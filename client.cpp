
#include "clients.hpp"
#include "response.hpp"
#include "cgi_response.hpp"


Client::Client(Server &server, int fd) : _server(server)
{
	_pfd.fd = fd;
	_pfd.events = POLLIN;
	_pfd.revents = 0;
	first_response_read = true;
}

void Client::save_request(string request)
{
		// TODO buffered requesting here
		// if (first_response_read)
		// {
			this->_response = new response(request, this->_server.get_locations());
			// first_response_read = false;
		// }
		// else 
		// {
		// 	_response->process_multipart(request);
		// }



		// if (check_cgi == "CGI")
		// {
		// 	cout << RED <<  "IS CGI" << RESET << endl;
		// 	this->_is_cgi = true;
		// 	_cgi.cgi_init(_response->get_script_path(), _response->get_body(), _response->prepare_cgi(this->_server));
		// }
	// }
	// if (_is_cgi)
	// {
	// 	if (_cgi.is_cgi_ready())
	// 	{
	// 		cgi_exit_code = _cgi.cgi_get_code();
	// 		if (cgi_exit_code != 200)
	// 		{
	// 			cgi_response cgi_resp("", cgi_exit_code);
	// 			_cgi_resp = cgi_resp.get_cgi_response();
	// 		}
	// 		else
	// 		{
	// 			cgi_response cgi_resp(_cgi.cgi_get_response(), cgi_exit_code);
	// 			_cgi_resp = cgi_resp.get_cgi_response();
	// 			_is_cgi = false;
	// 		}
	// 	}
	// }
	// else
	// {
	// 	this->_response = new response(request, this->_server.get_locations());
	// }

	// std::string check_cgi = this->_response->get_response_header();

}

pollfd& Client::get_fd()
{
	return (this->_pfd);
}

void Client::change_event()
{
	this->_pfd.events = POLLOUT;
}

void Client::change_event(int)
{
	this->_pfd.events = POLLIN;
}

// string Client::get_response()
// {
// 	return (_response);
// }

Client::Client(const Client &other)	
: _server(other._server), _request(other._request), _pfd(other._pfd) ,_is_cgi(false), _headers_sended(false)
{

}

Client&	Client::operator=(const Client &other)
{
	if (this != &other)
	{
		_request = other._request;
		_pfd = other._pfd;
	}
	return *this;
}
