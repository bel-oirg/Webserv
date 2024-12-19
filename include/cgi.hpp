#ifndef CGI_HPP
#define CGI_HPP

#include "server.hpp"

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
		int 	code;
		script_type type;
		int		child_stat;
		char	**env;
		string request_body;
		FILE*	outfile;
		FILE*	infile;
		int		forked;
		// bool	is_cgi;

	public:
		Cgi(string _scriptpath, string _request_body, map<string, string> env_map);
		void 	cgi_run();
		void 	get_script_type();
		int 	cgi_get_code();
		string 	cgi_get_response();
		bool	is_cgi_ready();
		int 	get_outfd()
		{
			return (fileno(outfile));
		}
		~Cgi();

		// ~Cgi();
};


#endif /* CGI_HPP */
