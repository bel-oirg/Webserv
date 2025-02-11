#define MAX_URI_SIZE     2048
#define RW_BUFFER        2000
#define pp cout << 


#define RESET   "\033[0m"
#define GRAY    "\033[90m" 
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE  "\033[1;37m"

#include <string>
#include <algorithm>
#include <limits>
#include <ctime>
#include <cstdlib>
#include <stdint.h>
#include <iomanip>
#include <map>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <assert.h>
#include <netinet/in.h>
#include <set>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <poll.h>
#include <fstream>
#include <vector>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <sys/stat.h>
#include <dirent.h>

using namespace std; //TODO possible ?

using std::string;

string   fix_slash(string base, string file);
void     err_(const std::string &err);