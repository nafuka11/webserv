#include "ConfigParser.hpp"
#include <fstream>
#include <iostream>//del;
#include <string>
#include "Config.hpp"

ConfigParser::ConfigParser(Config &config) : config_(config)
{
}

ConfigParser::~ConfigParser()
{
}

void ConfigParser::readFile(const std::string &filepath)
{
    std::ifstream ifs(filepath);

    if (!ifs)
    {
        throw (OpenConfigFileException());
    }
    std::string line;

    while (std::getline(ifs, line))
    {
        std::vector<std::string> words = splitLine(line);
        /* del */
        for (size_t i = 0; i < words.size(); i++)
        {
            std::cout << i << ": [" << words.at(i) << "]" << std::endl;
        }
        std::cout << "----------------" << std::endl;
        /*******/
        // if (split_string[0]に"server"が見つかった)
            // if (split_string[1]に"{"が見つかった)
                // (ここで、それ以外の文字が[2]以降に入っていたらエラーとする？)
    }

    config_.~Config();//del
    ifs.close();
}

std::vector<std::string> ConfigParser::splitLine(const std::string &line)
{
    std::vector<std::string> words;
    size_t start = 0;
    size_t end = 0;

    while (line[start])
    {
        while (isspace(line[start]))
        {
            ++start;
        }
        end = start;
        while (isprint(line[end]) && !(isspace(line[end])))
        {
            ++end;
        }
        words.push_back(line.substr(start, (end - start)));
        start = end;
    }
    return (words);
}


const char *ConfigParser::OpenConfigFileException::what() const throw()
{
    //文言考える
    return ("ConfigParser::OpenConfigFileException");
}
