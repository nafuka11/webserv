#include "ConfigError.hpp"

const std::string ConfigError::SERVER_NAME = "webserv: ";

ConfigError::ConfigError(const ConfigErrorType error_type, const std::string &error_word,
                         const std::string &filepath, int line_pos)
{
    setErrorMessage(error_type, error_word, filepath, line_pos);
}

ConfigError::~ConfigError() throw()
{
}

void ConfigError::setErrorMessage(const ConfigErrorType error_type, const std::string &error_word,
                                  const std::string &filepath, int line_pos)
{
    switch (error_type)
    {
    case NO_OPEN_DIRECTIVE:
        error_message_ = SERVER_NAME + "directive \"" + error_word
                         + "\" has no opening “{” in " + filepath  + ":" + std::to_string(line_pos);
        break;
    case NOT_ALLOWED_DIRECTIVE:
        error_message_ = SERVER_NAME + error_message_ = "\"" + error_word
                         + "\" directive is not allowed here in " + filepath + ":" + std::to_string(line_pos);
        break;
    case UNKOWN_DIRECTIVE:
        error_message_ = SERVER_NAME + "unknown directive \"" + error_word
                         + "\" in " + filepath + ":" + std::to_string(line_pos);
        break;
    default:
        break;
    }
}

const char *ConfigError::ConfigError::what()  const throw()
{
    return error_message_.c_str();
}
