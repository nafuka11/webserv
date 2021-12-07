#include "SystemError.hpp"
#include <cstring>

SystemError::SystemError(const std::string &function_name, int err_no)
    : std::runtime_error(function_name + ": " + std::string(strerror(err_no)))
{
}
