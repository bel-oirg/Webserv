#include "cgi.hpp"

int Cgi::cgi_get_code()
{
	return this->code;
}

string Cgi::cgi_get_response()
{
	return this->response;
}

void Cgi::get_script_type()
{
	string extention;
	size_t dot_pos = script_path.find_last_of('.');

	// if there is no dot its binary
	if (dot_pos == string::npos)
	{
		this->type = binary;
	}

	extention = script_path.substr(dot_pos);

	if (extention == ".ruby")
		this->type = ruby;
	else if (extention == ".sh")
		this->type = shell;
	else if (extention == ".py")
		this->type = python;
	else if (extention == ".php")
		this->type = php;
	else
		this->type = unknown;
}

void Cgi::cgi_run()
{
	if (child_stat == 0)
	{
		char *args[3];
		switch (this->type)
		{
		case python:
			args[0] = (char *)"/usr/bin/python";
			break;
		case shell:
			args[0] = (char *)"/bin/sh";
			break;
		case ruby:
			args[0] = (char *)"/usr/bin/ruby";
			break;
		case php:
			args[0] = (char *)"/usr/bin/php";
			break;
		case binary:
			args[0] = (char *)script_path.c_str();
			break;
		default:
			throw 1; 
		}
		args[1] = (char *)script_path.c_str();
		args[2] = NULL;

		fdout = fileno(tmpfile());
		fdin = fileno(tmpfile());

		write(fdin, request_body.c_str(), request_body.size());
		lseek(fdin, 0, SEEK_SET);

		child_stat = 1;
		forked = fork();
		if (forked == -1)
		{
			perror("fork() faild");
			code = 500;
			return;
		}

		if (forked == 0)
		{
			alarm(4); // TODO get time from configfile
			dup2(fdin, STDIN_FILENO);
			dup2(fdout, STDOUT_FILENO);
			close(fdin);
			close(fdout);
			execve(args[0], args, env);
			perror("execve() faild");
			exit(1);
		}
		cout << GREEN << "cgi is running" << RESET << endl;
	}
	else if (child_stat == 1)
	{
		int status;

		// cout << MAGENTA << "checking the child again" << RESET << endl;
		if (waitpid(forked, &status, WNOHANG) != 0)
		{
			cout << YELLOW << "cgi is DONE" << RESET << endl;
			if (WIFSIGNALED(status))
			{
				if (WTERMSIG(status) == SIGALRM)
				{
					this->code = 504;
					clog << "Cgi timeout " << endl;
				}
				else
				{
					this->code = 500;
					clog << "Cgi killed by deleverd signal [" << WTERMSIG(status) << "]" << endl;
				}
				close(fdin);
				close(fdout);
				child_stat = 2;
				return;
			}
			else if (WIFEXITED(status))
			{
				if (WEXITSTATUS(status) == 0)
				{
					code = 200;
					char buffer[101];
					lseek(fdout, 0, SEEK_SET);
					while (true)
					{
						int readed = read(fdout, buffer, 100);
						if (readed < 0)
						{
							perror("read() failed");
							exit(1);
						}
						else if (readed == 0)
							break;
						buffer[readed] = '\0';
						response += buffer;
					}
				}
				else
				{
					clog << "CGI error : exit stat => [" << WEXITSTATUS(status) << "]" << endl;
					code = 500;
				}
				close(fdin);
				close(fdout);
				child_stat = 2;
				return;
			}
		}
	}
}

bool	Cgi::is_cgi_ready()
{
	cgi_run();
	if (child_stat == 2)
	{
		close(fdin);
		close(fdout);
		return (true);
	}
	return (false);
}

void	Cgi::cgi_init(string scriptpath, string _request_body, std::map<string ,string> env_map)
{
	this->child_stat = 0;
	this->env = new char*[env_map.size() + 1];

	int i = 0;
	for (map<string, string>::iterator it = env_map.begin(); it != env_map.end(); it++)
	{
		string combined;

		combined = it->first + "=" + it->second;
		this->env[i] = new char[combined.size() + 1];
		strcpy(env[i], combined.c_str());
		i++;
	}

	this->env[i] = NULL;


	this->script_path = scriptpath;
	this->request_body = _request_body;
	get_script_type();
	cgi_run();

	// BUG  free the env 
}


// Cgi::~Cgi() {
//     if (env) {
//         for (int i = 0; env[i]; ++i) {
//             delete[] env[i];
//         }
//         delete[] env;
//     }
//     // close(fdout);
//     // close(fdin);
// }
