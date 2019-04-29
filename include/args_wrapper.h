#ifndef ARGS_WRAPPER_H
#define ARGS_WRAPPER_H

#include <optional>

struct args_wrapper
{
    args_wrapper(std::string pth = "");

    enum Order
    {
        LESS,
        EQUAL,
        GREATER
    };

    struct size_order
    {
        size_order(Order o, std::size_t s)
            : comp(o), sz(s)
        {}

        Order comp = EQUAL;
        std::size_t sz;
    };

    std::string path_to_find;

    std::optional<std::size_t> inum;        // number of inode
    std::optional<std::string> name;        // name of file to search
    std::optional<size_order> fsize;        // size options
    std::optional<std::size_t> nlinks;      //
    std::optional<std::string> exec_path;   //

    bool valid_data = true; // пофиг на контракты пока что:)
};

#endif // ARGS_WRAPPER_H
