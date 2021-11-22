#ifndef ADDRESSINFOERROR_HPP
#define ADDRESSINFOERROR_HPP

#include <stdexcept>
#include <string>

class AddressInfoError : public std::runtime_error
{
public:
    AddressInfoError(const std::string &function_name, int error_code);
};

#endif /* ADDRESSINFOERROR_HPP */
