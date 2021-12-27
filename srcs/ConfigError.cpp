#include "ConfigError.hpp"

const std::string ConfigError::SERVER_NAME = "webserv: ";

ConfigError::ConfigError(const ConfigErrorType error_type, const std::string &error_word,
                         const std::string &filepath, int line_pos)
    : error_message_(SERVER_NAME), file_info_(" in " + filepath + std::to_string(line_pos))
{
    setErrorMessage(error_type, error_word);
}

ConfigError::~ConfigError() throw()
{
}

void ConfigError::setErrorMessage(const ConfigErrorType error_type, const std::string &error_word)
{
    switch (error_type)
    {
    case DUPLICATE_LOCATION:
        error_message_ += "duplicate location \"" + error_word + "\"" + file_info_;
        break;
    case INVALID_NUM_OF_ARGS:
        error_message_ += "invalid number of arguments in \"" + error_word + "\" directive" + file_info_;
        break;
    case NO_OPEN_DIRECTIVE:
        error_message_ += "directive \"" + error_word + "\" has no opening \"{\"" + file_info_;
        break;
    case NOT_ALLOWED_DIRECTIVE:
        error_message_ += "\"" + error_word + "\" directive is not allowed here" + file_info_;
        break;
    case UNEXPECTED:
        error_message_ += "unexpected \"" + error_word + "\"" + file_info_;
        break;
    case UNEXPECTED_END:
        error_message_ += "unexpected end of file, expecting \"}\"" + file_info_;
        break;
    case UNKOWN_DIRECTIVE:
        error_message_ += "unknown directive \"" + error_word + "\"" + file_info_;
        break;
    default:
        break;
    }
}

const char *ConfigError::ConfigError::what()  const throw()
{
    return error_message_.c_str();
}
