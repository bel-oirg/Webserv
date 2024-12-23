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

void Cgi::load_cgi_script()
{
	string extention;
	size_t dot_pos;

	dot_pos = script_path.find('.');
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
		return;
	}

	if (find(location.cgi_extentions.begin(), location.cgi_extentions.end(), extention) == location.cgi_extentions.end())
	{
		code = 403; // TODO code for the extention no impl..
		child_stat = 2;
		return;
	}

	this->excutor = it->second;
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
			// char const * *argv = args;
			alarm(3); // Set timeout for CGI execution

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
		if (waitpid(forked, &status, WNOHANG) != 0)
		{

			if (WIFSIGNALED(status))
			{
				if (WTERMSIG(status) == SIGALRM)
				{
					this->code = 504;
					std::cerr << "CGI timeout" << std::endl;
				}
				else
				{
					this->code = 500;
					std::cerr << "CGI killed by signal [" << WTERMSIG(status) << "]" << std::endl;
				}

				child_stat = 2;
				return;
			}
			else if (WIFEXITED(status))
			{
				if (WEXITSTATUS(status) == 0)
				{
					code = 200;
				}
				else
				{
					std::cerr << "CGI error : exit status => [" << WEXITSTATUS(status) << "]" << std::endl;
					code = 500;
				}
				child_stat = 2;
				return;
			}
		}
	}
}

bool Cgi::is_cgi_ready()
{
	cgi_run();
	if (child_stat == 2)
	{
		return (true);
	}
	return (false);
}

Cgi::Cgi(string _scriptpath, string _request_body, map<string, string> env_map, loc_details &cur_loc, loc_details &def_loc)
	: response(""),
	  script_path(_scriptpath),
	  code(0),
	  child_stat(0),
	  env(new char *[env_map.size() + 1]),
	  args(),
	  request_body(_request_body),
	  outfile(NULL),
	  infile(NULL),
	  forked(0),
	  location(cur_loc),
	  defa_ult(def_loc),
	  excutor("")
{
	load_cgi_script();

	/*
		FIXME POST REQUEST ON CGI WITH A BODY DOES NOT WORK, try cgi-bin/name.py
	*/

	int i = 0;
	for (map<string, string>::iterator it = env_map.begin(); it != env_map.end(); ++it)
	{
		string combined;

		combined = it->first + "=" + it->second;
		this->env[i] = new char[combined.size() + 1];
		strcpy(env[i], combined.c_str());
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
		if (fd != -1)
			close(fd);
		fclose(infile);
	}
	if (outfile)
	{
		int fd = fileno(outfile);
		if (fd != -1)
			close(fd);
		fclose(outfile);
	}
}
