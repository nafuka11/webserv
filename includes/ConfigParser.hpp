#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <exception>
#include <string>

class Config;

class ConfigParser
{
public:
    ConfigParser(Config &config);
    ~ConfigParser();
    void readFile(const std::string &file_path);
    std::vector<std::string> splitLine(const std::string &line/*, const std::vector<std::string> delims*/);
private:
    class OpenConfigFileException : public std::exception
    {
        public:
            virtual const char *what() const throw();
    };

    Config &config_;
};

#endif /* CONFIGPARSER_HPP */
