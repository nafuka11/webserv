#include "AddressInfoError.hpp"
#include <netdb.h>

AddressInfoError::AddressInfoError(const std::string &function_name, int error_code)
    : std::runtime_error(function_name + ": " + std::string(gai_strerror(error_code)))
{
}
