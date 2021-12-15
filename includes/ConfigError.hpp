#ifndef CONFIGERROR_HPP
#define CONFIGERROR_HPP

#include <exception>
#include <string>
#include "ConfigErrorType.hpp"

class ConfigError : public std::exception
{
public:
    ConfigError(const ConfigErrorType error_type, const std::string &error_word, const std::string &filepath, int num_line);
    virtual ~ConfigError() throw();
    virtual const char *what() const throw();
private:
    std::string error_message_;
};

#endif /* CONFIGERROR_HPP */
