#include "ConfigParser.hpp"
#include <fstream>
#include "Config.hpp"

ConfigParser::ConfigParser(Config &config) : config_(config)
{
}

ConfigParser::~ConfigParser()
{
}

void ConfigParser::readFile(const std::string &file_path)
{
    std::ifstream ifs(file_path);

    if (!ifs)
    {
        throw (OpenConfigFileException());
    }
    config_.~Config();//del
    ifs.close();
}

const char *ConfigParser::OpenConfigFileException::what() const throw()
{
    //文言考える
    return ("ConfigParser::OpenConfigFileException");
}
