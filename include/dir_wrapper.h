#ifndef DIR_WRAPPER_H
#define DIR_WRAPPER_H

#include <string>

struct dir_wrapper
{
    dir_wrapper(const std::string &name);
    ~dir_wrapper();

    int file_descriptor;
    bool available = true;

private:
    void print_error(const std::string &s = "");

};

#endif // DIR_WRAPPER_H
