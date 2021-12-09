#include "ConfigParser.hpp"
#include <fstream>
#include <iostream>//del;
#include <string>
#include "Config.hpp"

const std::string SERVER_BLOCK_DIRECTIVE[3] = {
    "listen",
    "server_name"
};

// void (*parse_directive_func[3])(ServerConfig&, const std::vector<std::string>&) = {
// 	&ConfigParser::parseListenDirective,
// 	&ConfigParser::parseServerNameDirective,
// };

ConfigParser::ConfigParser(Config &config) : config_(config), state_(PARSE_MAIN_BLOCK)
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
        throw OpenConfigFileException();
    }
    std::string line;

    while (std::getline(ifs, line))
    {
        const std::vector<std::string> words = splitLine(line);

        /* 後で消す **************/
        for (size_t i = 0; i < words.size(); i++)
        {
            std::cout << i << ": [" << words.at(i) << "]" << std::endl;
        }
        std::cout << "----------------" << std::endl;
        /************************/
        parseWords(words);
        /* parseLine()  state_ = MAIN_BLCK(init) , SERVER_BLOCK_START, SERVER_BLOCK_DONE, LOCATION_BLOCK_START, LOCATION_BLOCK_DONE, DIRECTIVE_OK*/
        // if (split_string[0]に"server"が見つかった
            // if (split_string[1]に"{"が見つかった)
                // state_ = SERVER_BLOCK_START
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

void ConfigParser::parseWords(const std::vector<std::string> &words)
{
    switch (state_)
    {
    case PARSE_MAIN_BLOCK:
        std::cout << "PARSE_MAIN_BLOCK: " << words[0] << std::endl;
        parseMainBlock(words);
        state_ = PARSE_SERVER_BLOCK;//del
        break;
    case PARSE_SERVER_BLOCK:
        // std::cout << "PARSE_SERVER_BLOCK: " << words[0] << std::endl;
        // parseServerBlock(words);
        state_ = PARSE_LOCATION_BLOCK;//del
        break;
    case PARSE_LOCATION_BLOCK:
    //     std::cout << "PARSE_LOCATION_BLOCK: " << words[0] << std::endl;
    //     parseLocationBlock(words);
        state_ = PARSE_DONE;//del;
        break;
    default:
        break;
    }
}

void ConfigParser::parseMainBlock(const std::vector<std::string> &words)
{
    if (words[0] == "server")
    {
        if (words[1] == "{" && words.size() == 2)
        {
            state_ = PARSE_SERVER_BLOCK;
            return ;
        }
        else
        {
            throw ErrorMainBlockException();
        }
    }
    // else if... 配列でmainブロック内に記載可能なディレクティブを持って、処理関数を紐付け。でもいいかも
}

// void ConfigParser::parseServerBlock(const std::vector<std::string> &words)
// {
    // ServerConfig server = ServerConfig();

    // for (int i = 0; i < 3; ++i)
    // {
    //     if (words[0] == SERVER_BLOCK_DIRECTIVE[i])
    //     {
    //         parse_directive_func[i](server, words);
    //     }
    // }
// }

// void ConfigParser::parseLocationBlock(const std::vector<std::string> &words)
// {

//     return ;
// }

void ConfigParser::parseListenDirective(ServerConfig &server, const std::vector<std::string> &words)
{
    server.setListen(std::atoi(words[1].c_str())); // ConfigAtoi作るか?
    config_.addServer(server);
}
void ConfigParser::parseServerNameDirective(ServerConfig &server, const std::vector<std::string> &words)
{
    server.setServerName(words[1]);
    config_.addServer(server);
}

const char *ConfigParser::OpenConfigFileException::what() const throw()
{
    //文言考える
    return ("ConfigParser::OpenConfigFileException");
}

const char *ConfigParser::ErrorMainBlockException::what() const throw()
{
    //文言考える
    return ("ConfigParser::ErrorMainBlockException");
}
