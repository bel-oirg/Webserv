#include "webserv.hpp"
#include "clients.hpp"
#include "response.hpp"

Client::Client(int fd_serv, int fd, vector<Server> *servers, int port)
	: _response(NULL),
	  _headers_sended(false),
	  first_response_read(true),
	  handshake(true),
	  servers(servers),
	  server_fd(fd_serv),
	  _request_buffer(""),
	  _last_interaction()
{
	entry_port = port;
	_pfd.fd = fd;
	_pfd.events = POLLIN | POLLOUT;
	_pfd.revents = 0;
	this->register_interaction();
}

bool headers_complet(string &request)
{
	if (request.find("boundary=") != string::npos)
	{
		if (request.find("filename=") != string::npos) 
			return true;
		else
			return false;
	}
	else if (request.find("\r\n\r\n") != string::npos)
	{
		return (true);
	}
	return (false);
}

void Client::save_request(string request)
{
	if (first_response_read)
	{
		_request_buffer += request;
		if (headers_complet(_request_buffer))
		{
			this->_response = new response(_request_buffer, *servers, this->entry_port);
			first_response_read = false;
			_request_buffer.clear();
		}
	}
	else
	{
		_response->process_multipart(request);
	}
}

pollfd &Client::get_pollfd()
{
	return (this->_pfd);
}

void Client::listen_to_write()
{
	this->_pfd.events = POLLOUT;
}

void Client::register_interaction()
{
	this->_last_interaction = time(NULL);
}

time_t Client::get_last_interaction()
{
	return (this->_last_interaction);
}

string& Client::request_buffer()
{
	return (_request_buffer);
}

// Server&	Client::server()
// {
// 	// return (this->_server);
// }

Client::~Client()
{
	if (_response)
		delete _response;
}