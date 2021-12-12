#include "ConfigParser.hpp"
#include <fstream>
#include <iostream>//del;
#include <string>
#include "Config.hpp"
#include "MainConfig.hpp"
#include "SystemError.hpp"

ConfigParser::ConfigParser(Config &config)
: num_line_(1),
  config_(config),
  state_(CONF_CONTEXT_MAIN)
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
    readAndSplitFile(ifs);
    putSplitLines();// 後で消す
    ifs.close();
    parseFile();
}

void ConfigParser::readAndSplitFile(std::ifstream &ifs)
{
    std::string line;

    while (std::getline(ifs, line))
    {
        const std::vector<std::string> words = splitLine(line);
        config_file_.push_back(words);
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
void ConfigParser::putSplitLines()
{
    size_t line_num = 1;
    for (std::vector<std::vector<std::string> > ::const_iterator vviter = config_file_.begin();
         vviter != config_file_.end();
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

void ConfigParser::parseFile()
{
    for (std::vector<std::vector<std::string> > ::const_iterator vviter = config_file_.begin();
         vviter != config_file_.end();
         ++vviter)
    {
        parseMainContext();
        ++num_line_;
    }
}

void ConfigParser::parseMainContext()
{
    if (isServerContext())
    {
        state_ = CONF_CONTEXT_SERVER;
        std::cout << "Line." << (num_line_ + 1) << ": state " << state_ << std::endl; // 後で消す
        return ;
    }
}

// void ConfigParser::parseServerContext()
// {
//     MainConfig main = MainConfig();//parseMainContextで作成したオブジェクトを引数でもらう
//     ServerConfig server = ServerConfig(main);

//     for (; num_line_ < config_file_.size(); ++num_line_)
//     {
//          std::cout << "Line." << (num_line_ + 1) << std::endl; // 後で消す
//         if (config_file_.at(num_line_).size() == 0)
//         {
//             continue ;
//         }
//         if ((config_file_.at(num_line_).at(0) == "}")
//              && (config_file_.at(num_line_).size() == 1))
//         {
//             config_.addServer(server);
//             state_ = CONF_CONTEXT_MAIN;
//             return ;
//         }
//         else if (isLocationContext(config_file_))
//         {
//             state_ = CONF_CONTEXT_LOCATION;
//         //parseLocationContext()
//             std::cout << "Line." << (line_num + 1) << ": state " << state_ << std::endl; // 後で消す
//             for (; line_num < config_file_.size(); ++line_num)
//             {
//                 if ((config_file_.at(line_num).at(0) == "}")
//                     && (config_file_.at(line_num).size() == 1))
//                 {
//                     // parseLocationContext(...);
//                     break ;
//                 }
//             }
//         }
//         else if (config_file_.at(line_num).at(0) == "listen")
//         {
//             //構文チェック(引数は正しいか、最後に";"があるか)
//             server.setListen(atoi(config_file_.at(line_num).at(1).c_str()));
//         }
//         else if (config_file_.at(line_num).at(0) == "server_name")
//         {
//             //構文チェック(引数は正しいか、最後に";"があるか)
//             server.setServerName(config_file_.at(line_num).at(1));
//         }
//     }
// }

// bool ConfigParser::isServerContext()
// {
//     if ((config_file_.at(line_num).at(0) == "server")
//          && (config_file_.at(line_num).at(1) == "{")
//          && (config_file_.at(line_num).size() == 2))
//     {
//         return true;
//     }
//     return false;
// }

// bool ConfigParser::isLocationContext()
// {
//     if ((config_file_.at(line_num).at(0) == "location")
//               && (config_file_.at(line_num).at(2) == "{")
//               && (config_file_.at(line_num).size() == 3))
//     {
//         return true;
//     }
//     return false;
// }

const char *ConfigParser::ErrorBlockStartException::what() const throw()
{
    //文言考える
    return ("ConfigParser::ErrorBlockStartException");
}
