#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <iostream>

#include "dir_wrapper.h"

dir_wrapper::dir_wrapper(const std::string &name)
{
    file_descriptor = open(name.c_str(), O_RDONLY | O_DIRECTORY);
    if (file_descriptor == -1)
    {
        available = false;
        print_error(std::string("Fail to open: ") + name + ". ");
    }
}

dir_wrapper::~dir_wrapper()
{
    if (file_descriptor != -1)
    {
        const auto result = close(file_descriptor);
        if (result == -1)
        {
            print_error();
        }
    }
}

void dir_wrapper::print_error(const std::string &s)
{
    std::cerr << s << strerror(errno) << std::endl;
}
