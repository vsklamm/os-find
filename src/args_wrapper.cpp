#include "args_wrapper.h"

args_wrapper::args_wrapper(std::string pth)
    : path_to_find(std::move(pth))
{
}
