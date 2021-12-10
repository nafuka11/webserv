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

// 必要なくなったら消す
void ConfigParser::putSplitLine(std::vector<std::vector<std::string> > &config_file)
{
    for (size_t line_num = 0; line_num < config_file.size(); ++line_num)
    {
        std::cout << (line_num + 1) << ")------------------" << std::endl;
        for (size_t word = 0; word < config_file.at(line_num).size(); ++word)
        {
            std::cout << word << ": [" << config_file.at(line_num).at(word) << "]" << std::endl;
        }
    }
}
// 必要なくなったら消す

void ConfigParser::readFile(const std::string &filepath)
{
    std::ifstream ifs(filepath);
    if (!ifs)
    {
        throw SystemError("open", errno);
    }

    std::vector<std::vector<std::string> > config_file;
    readAndSplit(ifs, config_file);
    putSplitLine(config_file);//del
    ifs.close();

    for (size_t line_num = 0; line_num < config_file.size(); ++line_num)
    {
        if (config_file.at(line_num).size() == 0)
        {
            continue ;
        }
        if ((config_file.at(line_num).at(0) == "server")
            && (config_file.at(line_num).at(1) == "{")
            && (config_file.at(line_num).size() == 2))
        {
                std::cout << "Line." << (line_num + 1) << ": FIND SERVER" << std::endl;//del

                // state_ = SERVER_BLOCK;
                ++line_num;
                ServerConfig server = ServerConfig();
                for (; line_num < config_file.size(); ++line_num)
                {
                    for (size_t word = 0; word < config_file.at(line_num).size(); ++word)
                    {
                        if ((config_file.at(line_num).at(word) == "}")
                             && (config_file.at(line_num).size() == 1))
                        {
                            //state_SERVER_END;
                            config_.addServer(server);
                            break ;
                        }
                        else if (config_file.at(line_num).at(word) == "listen")
                        {
                            //構文チェック(引数は正しいか、最後に";"があるか)
                            ++word;
                            server.setListen(atoi(config_file.at(line_num).at(word).c_str()));
                        }
                        else if (config_file.at(line_num).at(word) == "server_name")
                        {
                            //構文チェック(引数は正しいか、最後に";"があるか)
                            ++word;
                            server.setServerName(config_file.at(line_num).at(word));
                        }
                    }
                }
        }
    }
}
void ConfigParser::readAndSplit(std::ifstream &ifs, std::vector<std::vector<std::string> > &config_file)
{
    std::string line;

    for (int line_num = 0; std::getline(ifs, line); ++line_num)
    {
        const std::vector<std::string> words = splitLine(line);
        config_file.push_back(words);
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

const char *ConfigParser::ErrorBlockStartException::what() const throw()
{
    //文言考える
    return ("ConfigParser::ErrorBlockStartException");
}
