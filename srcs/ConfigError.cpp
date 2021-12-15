#include "ConfigError.hpp"

#include <iostream>//del

ConfigError::ConfigError(const ConfigErrorType error_type, const std::string &error_word, const std::string &filepath, int num_line)
//: error_message_(error_type + " \"" + error_word + "\" in " + filepath + ":" + std::to_string(num_line))
{
    switch (error_type)
    {
    case UNKNOWN_DIRECTIVE:
        error_message_ = "unknown directive \"" + error_word + "\" in " + filepath + ":" + std::to_string(num_line);
        break;
    case NOT_ALLOWED_DIECTIVE:
        error_message_ = "\"" + error_word + "\" directive is not allowed here in " + filepath + ":" + std::to_string(num_line);
        break;
    default:
        break;
    }
}

ConfigError::~ConfigError() throw()
{
}

const char *ConfigError::ConfigError::what() const throw()
{
    return error_message_.c_str();
}
