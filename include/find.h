#ifndef H_FINDER
#define H_FINDER

#include <cstddef>
#include <iostream>
#include <string>

#include "args_wrapper.h"

namespace find
{

class finder
{
public:

    finder() = default;

    void init(args_wrapper arguments);
    void run(std::ostream& ocout = std::cout, std::ostream& oerr = std::cerr) const;

private:

    void process_dir(const std::string &directory_name) const;
    void process_dir_entry(const std::string &filename, const std::string &path, const struct stat &stat_info) const;
    void execute(const std::string filename, const std::string arg) const;
    std::string resolve_path(const std::string &pref, const std::string &post) const;
    bool dot_or_two_dots(const std::string &dir) const;

    void print_found_file(const std::string &s) const;
    void print_error(const std::string &s = "") const;

    args_wrapper arguments;
    const std::size_t buf_size = 1024*1024*5;

};

} // namespace find

#endif // H_FINDER
