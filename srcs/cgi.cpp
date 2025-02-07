#include "cgi.hpp"
#include "webserv.hpp"

int Cgi::cgi_get_code()
{
	return this->code;
}

string Cgi::cgi_get_response()
{
	return this->response;
}

void	Cgi::load_cgi_script()
{
	string extention;
	size_t dot_pos;

	dot_pos = script_path.rfind('.');
	if (dot_pos == string::npos)
	{
		code = 500;
		child_stat = 2;
		return;
	}

	extention = script_path.substr(dot_pos + 1);

	map<string, string>::iterator it = defa_ult.cgi_excutor.find(extention);
	if (it == defa_ult.cgi_excutor.end())
	{
		code = 502;
		child_stat = 2;
		return ;
	}

	if (find(location.cgi_extentions.begin(), location.cgi_extentions.end(), extention) 
			== location.cgi_extentions.end())
	{
		code = 403;
		child_stat = 2;
		return;
	}
	this->excutor = it->second;

	if (access(excutor.c_str(), X_OK))
	{
		code = 500;
		child_stat = 2;
		return ;
	}

	this->args[0] = (char *)excutor.c_str();
	this->args[1] = (char *)script_path.c_str();
	this->args[2] = NULL;
}


void Cgi::cgi_run()
{
	if (child_stat == 0)
	{
		this->outfile = tmpfile();
		this->infile = tmpfile();
		if (outfile == NULL || infile == NULL)
		{
			code = 500;
			child_stat = 2;
			return;
		}

		write(fileno(infile), request_body.c_str(), request_body.size());
		lseek(fileno(infile), 0, SEEK_SET);

		child_stat = 1;
		forked = fork();
		if (forked == -1)
		{
			perror("fork() failed");
			code = 500;
			child_stat = 2;
			return;
		}

		if (forked == 0)
		{
			alarm(10); // Set timeout for CGI execution

			dup2(fileno(infile), STDIN_FILENO);
			dup2(fileno(outfile), STDOUT_FILENO);
			close(fileno(infile));
			close(fileno(outfile));
			execve(args[0], args, env);
			perror("execve() failed");
			exit(1);
		}
	}
	else if (child_stat == 1)
	{
		int status;
		int wait_t = waitpid(forked, &status, WNOHANG);
		if (wait_t == 0)
		{
			return ;
		}

		else if (WIFSIGNALED(status))
		{
			int signal = WTERMSIG(status);
			if (signal == SIGALRM)
			{
				this->code = 504;
				std::cerr << "cgi: timeout" << std::endl;
			}
			else
			{
				cerr << "Error: cgi killed by delivered signal: `" << signal << "`" << endl;
				this->code = 500;
			}
			child_stat = 2;
		}
		else
		{
			int exit_stat = WEXITSTATUS(status);
			if (exit_stat == 0)
			{
				code = 200;
			}
			else
			{
				cerr << "Error: cgi exited abnormally with: `" << exit_stat << "`" << endl;
				code = 500;
			}
			child_stat = 2;
		}
	}
}

bool Cgi::is_cgi_ready()
{
	cgi_run();
	if (child_stat == 2)
	{
		forked = 0;
		return (true);
	}
	return (false);
}

Cgi::Cgi(string _scriptpath, string _request_body, map<string, string> env_map, loc_details &cur_loc, loc_details &def_loc)
	: response(""),
	  script_path(_scriptpath),
	  request_body(_request_body),
	  excutor(""),
	  code(0),
	  child_stat(0),
	  forked(0),
	  env(new char *[env_map.size() + 1]),
	  args(),
	  outfile(NULL),
	  infile(NULL),
	  location(cur_loc),
	  defa_ult(def_loc)
{
	load_cgi_script();

	int i = 0;
	for (map<string, string>::iterator it = env_map.begin(); it != env_map.end(); ++it)
	{
		string combined;

		combined = it->first + "=" + it->second;
		this->env[i] = new char[combined.size() + 1];
		strlcpy(env[i], combined.c_str(), combined.size() + 1);
		i++;
	}

	this->env[i] = NULL;
}

Cgi::~Cgi()
{
	for (int i = 0; env[i]; ++i)
	{
		delete[] env[i];
	}
	delete[] env;

	if (infile)
	{
		int fd = fileno(infile);
		close(fd);
		fclose(infile);
	}
	if (outfile)
	{
		int fd = fileno(outfile);
		close(fd);
		fclose(outfile);
	}
}

int 	Cgi::get_outfd()
{
	if (outfile)
		return (fileno(outfile));
	return -1;
}

int Cgi::get_pid()
{
	return forked;
}