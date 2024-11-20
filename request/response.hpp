#pragma once

#include <unordered_map>
#include <sys/stat.h>
#include <dirent.h>
#include <iostream>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>

class response
{
    private:
        std::unordered_map<int, std::string> status;
        std::string method;
        int stat_code;

    
    public:
        void fill_status();

};

