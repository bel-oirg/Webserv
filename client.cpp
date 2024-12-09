
#include "clients.hpp"
#include "response.hpp"
#include "cgi_response.hpp"


Client::Client(Server &server, int fd) : _server(server)
{
	_pfd.fd = fd;
	_pfd.events = POLLIN;
	_pfd.revents = 0;
}

void Client::save_request(string request)
{
	if (!_is_cgi)
	{
		this->_request = request;
		response resp(_request, _server.get_locations());
		this->_response = resp.get_response();
		if (_response == "CGI")
		{
			cout << RED <<  "IS CGI" << RESET << endl;
			this->_is_cgi = true;
			_response.clear();
			_cgi.cgi_init(resp.get_script_path(), resp.get_body(), resp.prepare_cgi(this->_server));
		}
	}
	if (_is_cgi)
	{
		if (_cgi.is_cgi_ready())
		{
			cgi_exit_code = _cgi.cgi_get_code();
			if (cgi_exit_code != 200)
			{
				cgi_response cgi_resp("", cgi_exit_code);
				_response = cgi_resp.get_cgi_response();
			}
			else
			{
				cgi_response cgi_resp(_cgi.cgi_get_response(), cgi_exit_code);
				_response = cgi_resp.get_cgi_response();
				_is_cgi = false;
			}
		}
	}

	this->_request = request;
	response resp(_request, _server.get_locations());
	this->_response = resp.get_response();
}

pollfd& Client::get_fd()
{
	return (this->_pfd);
}

void Client::change_event()
{
	this->_pfd.events = POLLOUT;
}

string Client::get_response()
{
	return (_response);
}

Client::Client(const Client &other)	
: _server(other._server), _request(other._request), _pfd(other._pfd) ,_is_cgi(false)
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
