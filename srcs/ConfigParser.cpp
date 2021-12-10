#include "ConfigParser.hpp"
#include <fstream>
#include <iostream>//del;
#include <string>
#include "Config.hpp"
#include "SystemError.hpp"

const std::string SERVER_BLOCK_DIRECTIVE[3] = {
    "listen",
    "server_name"
};

ConfigParser::ConfigParser(Config &config) : config_(config)//, state_(PARSE_MAIN_BLOCK)
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
        throw SystemError("open", errno);
    }

    std::vector<std::vector<std::string> > loaded_file;
    readAndSplitFile(ifs, loaded_file);
    putSplitLines(loaded_file);//del
    ifs.close();
    parseFile(loaded_file);
}

void ConfigParser::readAndSplitFile(std::ifstream &ifs, std::vector<std::vector<std::string> > &loaded_file)
{
    std::string line;

    for (int line_num = 0; std::getline(ifs, line); ++line_num)
    {
        const std::vector<std::string> words = splitLine(line);
        loaded_file.push_back(words);
    }
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
        while (isprint(line[end]) && !(isspace(line[end])) && (line[end] != ';'))
        {
            ++end;
        }
        words.push_back(line.substr(start, (end - start)));
        start = end;
        if (line[start] == ';')
        {
            words.push_back(line.substr(start, 1));
            start++;
        }
    }
    return (words);
}

// 必要なくなったら消す
void ConfigParser::putSplitLines(std::vector<std::vector<std::string> > &loaded_file)
{
    for (size_t line_num = 0; line_num < loaded_file.size(); ++line_num)
    {
        std::cout << (line_num + 1) << ")------------------" << std::endl;
        for (size_t word = 0; word < loaded_file.at(line_num).size(); ++word)
        {
            std::cout << word << ": [" << loaded_file.at(line_num).at(word) << "]" << std::endl;
        }
    }
}
// 必要なくなったら消す

void ConfigParser::parseFile(std::vector<std::vector<std::string> > &loaded_file)
{
    for (size_t line_num = 0; line_num < loaded_file.size(); ++line_num)
    {
        if (loaded_file.at(line_num).size() == 0)
        {
            continue ;
        }
        if ((loaded_file.at(line_num).at(0) == "server")
            && (loaded_file.at(line_num).at(1) == "{")
            && (loaded_file.at(line_num).size() == 2))
        {
                std::cout << "Line." << (line_num + 1) << ": FIND SERVER" << std::endl;//del

                // state_ = SERVER_BLOCK;
                ++line_num;
                ServerConfig server = ServerConfig();
                for (; line_num < loaded_file.size(); ++line_num)
                {
                    for (size_t word = 0; word < loaded_file.at(line_num).size(); ++word)
                    {
                        if ((loaded_file.at(line_num).at(word) == "}")
                             && (loaded_file.at(line_num).size() == 1))
                        {
                            //state_SERVER_END;
                            config_.addServer(server);
                            break ;
                        }
                        else if (loaded_file.at(line_num).at(word) == "listen")
                        {
                            //構文チェック(引数は正しいか、最後に";"があるか)
                            ++word;
                            server.setListen(atoi(loaded_file.at(line_num).at(word).c_str()));
                        }
                        else if (loaded_file.at(line_num).at(word) == "server_name")
                        {
                            //構文チェック(引数は正しいか、最後に";"があるか)
                            ++word;
                            server.setServerName(loaded_file.at(line_num).at(word));
                        }
                    }
                }
        }
    }
}

const char *ConfigParser::ErrorBlockStartException::what() const throw()
{
    //文言考える
    return ("ConfigParser::ErrorBlockStartException");
}
