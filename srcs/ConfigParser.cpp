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

void ConfigParser::readFile(const std::string &file_path)
{
    std::ifstream ifs(file_path);

    if (!ifs)
    {
        throw (OpenConfigFileException());
    }
    std::string line;

    while (std::getline(ifs, line))
    {
        // line内の一行文字列をsplit
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

std::vector<std::string> ConfigParser::splitLine(const std::string &line/*, const std::vector<std::string> delims*/)
{
    std::vector<std::string> words;
    size_t start = 0;
    size_t end = 0;

    while (line[end])
    {
        while (isspace(line[start]))
        {
            // std::cout  << "start[" << start << "]:" << line[start] << std::endl; //del
            ++start;
        }
        end = start;
        while (isprint(line[end]))
        {
            // std::cout  << "end[" << end << "]:" << line[end] << std::endl; //del
            if (isspace(line[end]))
                break ;
            ++end;
        }
        // std::cout  << "start[" << start << "]:" << line[start] << "| "//del
        //            << " end[" << end << "]:" << line[end] << "|" << std::endl; //del
        // for (size_t i = start; i <= end; ++i)
        // {
        //     std::cout << "[" << line[i] << "]";
        // }
        // std::cout << std::endl;//del;

        words.push_back(line.substr(start, (end - start)));
        if (line[end] == '\n')
        {
            break ;
        }
        start = end;
    }
    return (words);
}


const char *ConfigParser::OpenConfigFileException::what() const throw()
{
    //文言考える
    return ("ConfigParser::OpenConfigFileException");
}
