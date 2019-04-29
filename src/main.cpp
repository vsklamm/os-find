#include <algorithm>
#include <cstddef>
#include <regex>
#include <string>
#include <vector>

#include "args_wrapper.h"
#include "find.h"

const std::regex size_pattern{R"(^[\-\+\=]size$)"};

args_wrapper parse_arguments(int argc, char * argv[])
{
    std::vector<std::string> options;
    options.reserve(argc);
    if (argc < 2)
    {
        return {};
    }
    for (auto i = 1; i < argc; ++i)
    {
        options.emplace_back(argv[i]);
    }
    bool invalid_args = false;
    args_wrapper res(options[0]);
    for (size_t i = 1; i < options.size(); ++i)
    {
        if (i + 1 >= options.size())
        {
            invalid_args = true;
            break;
        }
        if (options[i] == "-inum")
        {
            std::size_t inm;
            std::stringstream(options[i + 1]) >> inm;
            res.inum = inm;
            ++i;
        }
        else if (options[i] == "-name")
        {
            res.name = std::move(options[i + 1]);
            ++i;
        }
        else if (options[i] == "-size")
        {
            std::size_t sz;
            using namespace std::string_view_literals;
            const auto ch = options[i + 1][0];
            const auto found = std::string_view("-=+").find(ch);
            auto ord = args_wrapper::Order::EQUAL;
            if (found != std::string::npos)
            {
                ord = args_wrapper::Order(int(found));
                std::stringstream(options[i + 1].substr(1)) >> sz;
            }
            else
            {
                std::stringstream(options[i + 1]) >> sz;
            }
            args_wrapper::size_order ord_sz(ord, sz);
            res.fsize = ord_sz;
            ++i;
        }
        else if (options[i] == "-nlinks")
        {
            std::size_t lnk;
            std::stringstream(options[i + 1]) >> lnk;
            res.nlinks = lnk;
            ++i;
        }
        else if (options[i] == "-exec")
        {
            res.exec_path = std::move(options[i + 1]);
            ++i;
        }
        else
        {
            invalid_args = true;
            break;
        }
    }
    res.valid_data = !invalid_args;
    return res;
}

int main(int argc, char * argv[])
{
    const auto parsed = parse_arguments(argc, argv);

    using namespace find;
    finder fnd;
    fnd.init(parsed);
    fnd.run();
}


