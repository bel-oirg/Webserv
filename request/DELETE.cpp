#include "DELETE.hpp"

bool DELETE::delete_all_folder_content(std::string ress_path)
{
    DIR* dir = opendir(ress_path.c_str());
    if (!dir)
        return(printf("Cannot open dir\n"), false);
    struct dirent *entry;
    struct stat s;
    std::string sub;

    while((entry == readdir(dir)))
    {
        sub = entry->d_name;
        if (sub == "." || sub == "..")
            continue;
        
        std::string a_path = ress_path + "/" + sub;
        if (stat(a_path.c_str(), &s) < 0)
            return (printf("stat fails\n"), closedir(dir), false);
        if (S_ISDIR(s.st_mode))
        {
            if (!delete_all_folder_content(a_path))
                return (closedir(dir), false);
        }
        else
        {
            if (remove(a_path.c_str()))
                return (printf("Cannot delete file\n"), closedir(dir), false);
        }
    }
    closedir(dir);
    if (rmdir(ress_path.c_str()))
        return (printf("Cannot delete dir\n"), false);
    return (true);
}

bool DELETE::has_write_access_on_folder()
{
    struct stat s;
    stat(ressource_path.c_str(), &s);
    return (s.st_mode & S_IWUSR);
}
