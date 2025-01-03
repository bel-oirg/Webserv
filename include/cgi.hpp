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
		// script_type type;
		int		child_stat;
		char	**env;
		char	*args[3];
		string 		request_body;
		FILE*		outfile;
		FILE*		infile;
		int			forked;
		string		key_path;
		loc_details &location;
		loc_details &defa_ult;
		string		excutor;
		// bool	is_cgi;

	public:
		Cgi(string _scriptpath, string _request_body, map<string, string> env_map, loc_details &cur_loc, loc_details &def_loc);
		void 	cgi_run();
		void	load_cgi_script();
		int 	cgi_get_code();
		string 	cgi_get_response();
		bool	is_cgi_ready();
		int 	get_outfd()
		{
			if (outfile)
				return (fileno(outfile));
			return -1;
		}
		int get_pid()
		{
			return forked;
		}
		~Cgi();
};


#endif /* CGI_HPP */
