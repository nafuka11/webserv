#include "ConfigParser.hpp"
#include <fstream>
#include <iostream>//del;
#include <string>
#include "Config.hpp"
#include "MainConfig.hpp"
#include "SystemError.hpp"

ConfigParser::ConfigParser(Config &config) : config_(config), state_(CONF_CONTEXT_MAIN)
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

    std::vector<std::vector<std::string> > config_file;

    readAndSplitFile(ifs, config_file);
    putSplitLines(config_file);// 後で消す
    ifs.close();
    parseFile(config_file);
}

void ConfigParser::readAndSplitFile(std::ifstream &ifs, std::vector<std::vector<std::string> > &config_file)
{
    std::string line;

    while (std::getline(ifs, line))
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

// 必要なくなったら消す
void ConfigParser::putSplitLines(std::vector<std::vector<std::string> > &config_file)
{
    size_t line_num = 1;
    for (std::vector<std::vector<std::string> > ::const_iterator vviter = config_file.begin();
         vviter != config_file.end();
         ++vviter)
    {
        std::cout << "------------------(" << line_num << std::endl;
        for (std::vector<std::string>::const_iterator viter = vviter->begin();
             viter != vviter->end();
             ++viter)
        {
            std::cout << "[" << *viter << "]" << std::endl;
        }
        ++line_num;
    }
    std::cout << "-----[end]-----------------------------------\n" << std::endl;
}
// 必要なくなったら消す

void ConfigParser::parseFile(std::vector<std::vector<std::string> > &config_file)
{
    size_t line_num = 1;
    for (std::vector<std::vector<std::string> > ::const_iterator vviter = config_file.begin();
         vviter != config_file.end();
         ++vviter)
    {
        std::cout << line_num << std::endl;
        ++line_num;
    }
}

void ConfigParser::parseMainContext(std::vector<std::vector<std::string> > &config_file, size_t &line_num)
{
    if (isServerContext(config_file, line_num))
    {
        state_ = CONF_CONTEXT_SERVER;
        std::cout << "Line." << (line_num + 1) << ": state " << state_ << std::endl; // 後で消す
        return ;
    }
}

void ConfigParser::parseServerContext(std::vector<std::vector<std::string> > &config_file, size_t &line_num)
{
    MainConfig main = MainConfig();//parseMainContextで作成したオブジェクトを引数でもらう
    ServerConfig server = ServerConfig(main);

    for (; line_num < config_file.size(); ++line_num)
    {
         std::cout << "Line." << (line_num + 1) << std::endl; // 後で消す
        if (config_file.at(line_num).size() == 0)
        {
            continue ;
        }
        if ((config_file.at(line_num).at(0) == "}")
             && (config_file.at(line_num).size() == 1))
        {
            config_.addServer(server);
            state_ = CONF_CONTEXT_MAIN;
            return ;
        }
        else if (isLocationContext(config_file, line_num))
        {
            state_ = CONF_CONTEXT_LOCATION;
        //parseLocationContext()
            std::cout << "Line." << (line_num + 1) << ": state " << state_ << std::endl; // 後で消す
            for (; line_num < config_file.size(); ++line_num)
            {
                if ((config_file.at(line_num).at(0) == "}")
                    && (config_file.at(line_num).size() == 1))
                {
                    // parseLocationContext(...);
                    break ;
                }
            }
        }
        else if (config_file.at(line_num).at(0) == "listen")
        {
            //構文チェック(引数は正しいか、最後に";"があるか)
            server.setListen(atoi(config_file.at(line_num).at(1).c_str()));
        }
        else if (config_file.at(line_num).at(0) == "server_name")
        {
            //構文チェック(引数は正しいか、最後に";"があるか)
            server.setServerName(config_file.at(line_num).at(1));
        }
    }
}

bool ConfigParser::isServerContext(std::vector<std::vector<std::string> > &config_file, size_t line_num)
{
    if ((config_file.at(line_num).at(0) == "server")
         && (config_file.at(line_num).at(1) == "{")
         && (config_file.at(line_num).size() == 2))
    {
        return true;
    }
    return false;
}

bool ConfigParser::isLocationContext(std::vector<std::vector<std::string> > &config_file, size_t line_num)
{
    if ((config_file.at(line_num).at(0) == "location")
              && (config_file.at(line_num).at(2) == "{")
              && (config_file.at(line_num).size() == 3))
    {
        return true;
    }
    return false;
}

const char *ConfigParser::ErrorBlockStartException::what() const throw()
{
    //文言考える
    return ("ConfigParser::ErrorBlockStartException");
}
