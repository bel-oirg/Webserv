#include "webserv.hpp"
#include "clients.hpp"
#include "response.hpp"

Client::Client(Server &server, int fd)
	: _server(server),
	  _response(NULL),
	  _headers_sended(false),
	  first_response_read(true),
	  _last_interaction(0),
	  _buff_num(0),
	  tmp_request("")
{
	_pfd.fd = fd;
	_pfd.events = POLLIN;
	_pfd.revents = 0;
}

bool	headers_complet(string &request)
{
	if (request.find("boundary=") != string::npos)
	{
		if (request.find("filename=") != string::npos)
			return true;
		else
			return false;
	}
	return (true);
}

void Client::save_request(string request)
{
	this->_buff_num++;
	if (first_response_read)
	{
		tmp_request += request;
		if (headers_complet(tmp_request))
		{
			this->_response = new response(tmp_request, this->_server.get_locations());
			first_response_read = false;
			tmp_request.clear();
		}
	}
	else
	{
		_response->process_multipart(request);
	}
}

pollfd &Client::get_fd()
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

void Client::register_interaction()
{
	this->_last_interaction = time(NULL);
}

clock_t Client::get_last_interaction()
{
	return (this->_last_interaction);
}

Client::Client(const Client &other)
	: _server(other._server)
	, _request(other._request)
	, _pfd(other._pfd)
	, _headers_sended(other._headers_sended)
	, _last_interaction(other._last_interaction)
{
}

Client &Client::operator=(const Client &other)
{
	if (this != &other)
	{
		_request = other._request;
		_pfd = other._pfd;
	}
	return *this;
}
