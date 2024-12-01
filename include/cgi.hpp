#include "server.hpp"


// TODO add the cgi to every server

enum script_type
{
	unknown,
	python = 1,
	shell,
	ruby,
	php,
	binary
};

class Cgi
{
	private:
		string response;
		string script_path;
		int code;
		script_type type;
		int		child_stat;
		char **env;
		string request_body;

	public:
		Cgi(string scriptpath, string _request_body, std::map<string ,string> env_map);
		// Cgi();
		void cgi_run();
		void get_script_type();
		int cgi_get_code();
		string cgi_get_response();
};