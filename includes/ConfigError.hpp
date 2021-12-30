#ifndef CONFIGERROR_HPP
#define CONFIGERROR_HPP

#include <exception>
#include <string>
#include "ConfigErrorType.hpp"

class ConfigError : public std::exception
{
public:
    ConfigError(const ConfigErrorType error_type, const std::string &error_word,
                const std::string &filepath, int line_pos);
    virtual ~ConfigError() throw();
    virtual const char *what() const throw();
private:
    static const std::string SERVER_NAME;

    void setErrorMessage(const ConfigErrorType error_type, const std::string &error_word);

    std::string error_message_;
    std::string file_info_;
};

#endif /* CONFIGERROR_HPP */
