#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <unistd.h>

#include <algorithm>
#include <cerrno>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <dirent.h> // Defines DT_* constants
#include <iostream>
#include <fcntl.h>
#include <functional>
#include <vector>

#include "dir_wrapper.h"
#include "find.h"

struct linux_dirent64
{
    /* ino64_t - Type of file serial numbers (LFS) */
    ino64_t         d_ino;
    off64_t         d_off;
    unsigned short  d_reclen;
    // unsigned char   d_type; // file type
    char const      d_name[1];
};

namespace find
{

void finder::init(args_wrapper args)
{
    arguments = std::move(args);
}

void finder::run(std::ostream& ocout, std::ostream& oerr) const
{
    if (!arguments.valid_data)
    {
        oerr << "Can't run with invalid arguments" << std::endl;
        return;
    }
    process_dir(arguments.path_to_find);
}

void finder::process_dir(const std::string &dir_name) const
{
    dir_wrapper dw(dir_name);
    if (!dw.available)
    {
        return;
    }
    char buf[buf_size];
    while(true)
    {
        const auto nread = syscall(SYS_getdents64, dw.file_descriptor, buf, buf_size);
        if (nread == -1 || nread == 0) // TODO: diff cases
        {
            std::cout << "Err or empty dir happen" << std::endl;
            break;
        }
        for (auto bpos = 0; bpos < nread;)
        {
            struct stat stat_info;
            const auto d = reinterpret_cast<linux_dirent64 *>(buf + bpos);
            const std::string entry(d->d_name);
            const std::string next_path = resolve_path(dir_name, entry);
            const char d_type = *(buf + bpos + d->d_reclen - 1);
            if (d_type == DT_DIR || !dot_or_two_dots(entry))
            {
                process_dir(next_path);
            }
            else if (d_type == DT_REG)
            {
                const auto stat_res = stat(next_path.c_str(), &stat_info);
                if (stat_res == -1)
                {
                    print_error(std::string("Cannot get info") + next_path + ": ");
                }
                else
                {
                    process_dir_entry(entry, next_path, stat_info);
                }
            }
            bpos += d->d_reclen;
        }
    }
}

void finder::process_dir_entry(const std::string &fname, const std::string &path, const struct stat &statinf) const
{
    if (arguments.nlinks.has_value() &&
            arguments.nlinks.value() != statinf.st_nlink) {
        return;
    }
    if (arguments.name.has_value() &&
            arguments.name.value() != fname) {
        return;
    }
    if (arguments.inum.has_value() &&
            arguments.inum.value() != statinf.st_ino) {
        return;
    }
    if (arguments.exec_path.has_value()) {
        execute(arguments.exec_path.value(), path);
        return;
    }
    if (arguments.fsize.has_value())
    {
        bool res = false;
        switch(arguments.fsize.value().comp)
        {
        case args_wrapper::Order::LESS:
            res = statinf.st_size < arguments.fsize.value().sz;
            break;
        case args_wrapper::Order::EQUAL:
            res = statinf.st_size == arguments.fsize.value().sz;
            break;
        case args_wrapper::Order::GREATER:
            res = statinf.st_size > arguments.fsize.value().sz;
            break;
        }
        if (res) {
            return;
        }
    }
    print_found_file(path);
}

void finder::execute(const std::string filename, const std::string arg) const
{
    auto caster = [](const std::string &s) { return const_cast<char*>(s.c_str()); };
    std::vector<char *> args{caster(filename), caster(arg), nullptr};

    pid_t pid = fork();
    if (pid <= -1)
    {
        print_error();
    }
    else if (pid == 0)
    {
        const auto exec_result = execve(filename.c_str(), args.data(), environ);
        if (exec_result == -1)
        {
            print_error(std::string("Cannot execute: \"") + filename + "\". ");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        int child_status;
        const auto wait_result = waitpid(pid, &child_status, 0);
        if (wait_result == -1)
        {
            print_error();
        }
    }
}

std::string finder::resolve_path(const std::string &pref, const std::string &post) const
{
    return pref + (pref.back() != '/' ? "/" : "") + post;
}

bool finder::dot_or_two_dots(const std::string &dir) const
{
    return dir.back() == '.' || dir.substr(dir.size() - 2) == "..";
}

void finder::print_found_file(const std::string &s) const
{
    std::cout << s << std::endl;
}

void finder::print_error(const std::string &s) const
{
    std::cerr << s << strerror(errno) << std::endl;
}

} // namespace find
