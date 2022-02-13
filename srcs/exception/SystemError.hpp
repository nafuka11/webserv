#ifndef SYSTEMERROR_HPP
#define SYSTEMERROR_HPP

#include <stdexcept>
#include <string>

class SystemError : public std::runtime_error
{
public:
    SystemError(const std::string &function_name, int err_no);
};

#endif /* SYSTEMERROR_HPP */
