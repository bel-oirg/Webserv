#ifndef CGI_HPP
#define CGI_HPP
#include "server.hpp"

class	Cgi
{
	private:
		string		script_path;
		string 		request_body;
		string		excutor;
		int 		code;
		int			child_stat;
		int			forked;
		char		**env;
		char		*args[3];
		FILE*		outfile;
		FILE*		infile;
		loc_details &location;
		loc_details &defa_ult;

	public:
		Cgi(string _scriptpath, string _request_body, map<string, string> env_map, loc_details &cur_loc, loc_details &def_loc);
		~Cgi();

		void 	cgi_run();
		void	load_cgi_script();
		int 	cgi_get_code();
		int 	get_outfd();
		int		get_pid();
		bool	is_cgi_ready();
};


#endif /* CGI_HPP */
