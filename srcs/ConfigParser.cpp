#include "ConfigParser.hpp"
#include <fstream>
#include <iostream>//del;
#include <string>
#include "Config.hpp"
#include "LocationConfig.hpp"
#include "MainConfig.hpp"
#include "SystemError.hpp"

const int ConfigParser::NUM_MAIN_DIRECTIVE = 7;
const int ConfigParser::NUM_SERVER_DIRECTIVE = 10;
const std::string ConfigParser::MAIN_DIRECTIVE[NUM_MAIN_DIRECTIVE] = {
    "allow_method",
    "autoindex",
    "cgi_extension",
    "client_max_body_size",
    "error_page",
    "index",
    "server"
};
const std::string ConfigParser::SERVER_DIRECTIVE[NUM_SERVER_DIRECTIVE] = {
    "allow_method",
    "autoindex",
    "client_max_body_size",
    "error_page",
    "index",
    "listen",
    "location",
    "return",
    "server_name",
    "upload_path"
};
const int ConfigParser::DIRECTIVE_NAME = 0;
const int ConfigParser::DIRECTIVE_VALUE = 1;
const int ConfigParser::SERVER_OPEN_BRACES = 1;

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
    readAndSplitLines(ifs);
    putSplitLines();// 後で消す
    ifs.close();
    // parseLines();
    parseMainContext();
}

void ConfigParser::readAndSplitLines(std::ifstream &ifs)
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
        if ((end - start) != 0)
            words.push_back(line.substr(start, (end - start)));
        start = end;
        if (line[start] == ';')
        {
            words.push_back(";");
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

// void ConfigParser::parseLines()
void ConfigParser::parseMainContext()
{
    MainConfig main_config = MainConfig();
    void (ConfigParser::*main_directive_func[NUM_MAIN_DIRECTIVE])(MainConfig&) = {
        &ConfigParser::parseAllowMethodDirective,
        &ConfigParser::parseAutoindexDirective,
        &ConfigParser::parseCgiExtensionDirective,
        &ConfigParser::parseClientMaxBodySizeDirective,
        &ConfigParser::parseErrorPageDirective,
        &ConfigParser::parseIndexDirective,
        &ConfigParser::parseServerContext
    };

    for (parse_line_ = config_file_.begin();
         parse_line_ != config_file_.end();
         ++parse_line_, ++num_line_)
    {
        if (parse_line_->size() == 0)
        {
            continue;
        }
        parse_line_word_ = parse_line_->begin();
        // parseMainContext(main_config);
        for (int i = 0; i < NUM_MAIN_DIRECTIVE; ++i)
        {
            if (parse_line_word_[DIRECTIVE_NAME] == MAIN_DIRECTIVE[i])
            {
                (this->*main_directive_func[i])(main_config);
            }
        }
    }
}

void ConfigParser::parseServerContext(MainConfig &main_config)
{
    std::cout << "Line." << num_line_ << " server" << std::endl; // 後で消す

    ServerConfig server_config = ServerConfig(main_config);
    void (ConfigParser::*server_directive_func[NUM_SERVER_DIRECTIVE])(ServerConfig&) = {
        &ConfigParser::parseAllowMethodDirective,
        &ConfigParser::parseAutoindexDirective,
        &ConfigParser::parseClientMaxBodySizeDirective,
        &ConfigParser::parseErrorPageDirective,
        &ConfigParser::parseIndexDirective,
        &ConfigParser::parseListenDirective,
        &ConfigParser::parseLocationContext,
        &ConfigParser::parseReturnDirective,
        &ConfigParser::parseServerNameDirective,
        &ConfigParser::parseUploadPath
    };
    ++parse_line_;
    ++num_line_;
    for (; parse_line_ != config_file_.end(); ++parse_line_, ++num_line_)
    {
        if (parse_line_->size() == 0)
        {
            continue;
        }
        parse_line_word_ = parse_line_->begin();
        // parseMainContext(main_config);
        if  (parse_line_word_[DIRECTIVE_NAME] == "}")
        {
            break;
        }
        for (int i = 0; i < NUM_SERVER_DIRECTIVE; ++i)
        {
            if (parse_line_word_[DIRECTIVE_NAME] == SERVER_DIRECTIVE[i])
            {
                std::cout << "Line." << num_line_ << " " << SERVER_DIRECTIVE[i] << std::endl; // 後で消す
                (this->*server_directive_func[i])(server_config);
            }
        }
    }
    // serverコンテキスト終了チェック

    // 許可しないディレクティブのエラー
    config_.addServer(server_config);
}

void ConfigParser::parseLocationContext(ServerConfig &server_config)
{
    LocationConfig location_config = LocationConfig(server_config);

    server_config.addLocation(parse_line_word_[1], location_config);
}

/*
    main config
*/
void ConfigParser::parseAllowMethodDirective(MainConfig &main_config)
{
    std::cout << "Line." << num_line_ << " allow_method" << std::endl; // 後で消す

    // 構文チェック

    main_config.clearAllowMethod();
    ++parse_line_word_;

    for (; parse_line_word_ != parse_line_->end(); ++parse_line_word_)
    {
        if (*parse_line_word_ == ";")
            break ;
        main_config.addAllowMethod(*parse_line_word_);
    }
}

void ConfigParser::parseAutoindexDirective(MainConfig &main_config)
{
    //構文チェック
    main_config.setAutoIndex(parse_line_word_[DIRECTIVE_VALUE]);
}

void ConfigParser::parseCgiExtensionDirective(MainConfig &main_config)
{
    //構文チェック
    main_config.setcgiExtension(parse_line_word_[DIRECTIVE_VALUE]);
}

void ConfigParser::parseClientMaxBodySizeDirective(MainConfig &main_config)
{
    //構文チェック
    main_config.setClientMaxBodySize(std::atoi(parse_line_word_[DIRECTIVE_VALUE].c_str()));
}

void ConfigParser::parseErrorPageDirective(MainConfig &main_config)
{
    //構文チェック

    std::vector<std::string>::const_iterator status_code;
    std::vector<std::string>::const_iterator uri;

    status_code = parse_line_->begin();
    ++status_code;
    uri = parse_line_->end();
    uri = uri - 2;
    // ex) error_page 404 /404.html;
    if (parse_line_->size() == 4)
    {
        main_config.clearErrorPage(std::atoi(status_code->c_str()));
        main_config.addErrorPage(std::atoi(status_code->c_str()), *uri);
        return ;
    }
    // ex) error_page 500 502 503 504 /50x.html;
    for (; status_code != uri; ++status_code)
    {
        main_config.clearErrorPage(std::atoi(status_code->c_str()));
        main_config.addErrorPage(std::atoi(status_code->c_str()), *uri);
    }
}

void ConfigParser::parseIndexDirective(MainConfig &main_config)
{
    // 構文チェック

    main_config.clearIndex();
    ++parse_line_word_;

   for (; parse_line_word_ != parse_line_->end(); ++parse_line_word_)
    {
        if (*parse_line_word_ == ";")
            break ;
        main_config.addIndex(*parse_line_word_);
    }
}

/*
    ServerConfig
*/
void ConfigParser::parseAllowMethodDirective(ServerConfig &server_config)
{
    // 構文チェック

    server_config.clearAllowMethod();
    ++parse_line_word_;

    for (; parse_line_word_ != parse_line_->end(); ++parse_line_word_)
    {
        if (*parse_line_word_ == ";")
            break ;
        server_config.addAllowMethod(*parse_line_word_);
    }
}
void ConfigParser::parseAutoindexDirective(ServerConfig &server_config)
{
    //構文チェック

    server_config.setAutoIndex(parse_line_word_[DIRECTIVE_VALUE]);
}

void ConfigParser::parseClientMaxBodySizeDirective(ServerConfig &server_config)
{
    //構文チェック
    server_config.setClientMaxBodySize(std::atoi(parse_line_word_[DIRECTIVE_VALUE].c_str()));
}

void ConfigParser::parseErrorPageDirective(ServerConfig &server_config)
{
    //構文チェック

    std::vector<std::string>::const_iterator status_code;
    std::vector<std::string>::const_iterator uri;

    status_code = parse_line_->begin();
    ++status_code;
    uri = parse_line_->end();
    uri = uri - 2;
    // ex) error_page 404 /404.html;
    if (parse_line_->size() == 4)
    {
        server_config.clearErrorPage(std::atoi(status_code->c_str()));
        server_config.addErrorPage(std::atoi(status_code->c_str()), *uri);
        return ;
    }
    // ex) error_page 500 502 503 504 /50x.html;
    for (; status_code != uri; ++status_code)
    {
        server_config.clearErrorPage(std::atoi(status_code->c_str()));
        server_config.addErrorPage(std::atoi(status_code->c_str()), *uri);
    }
}

void ConfigParser::parseIndexDirective(ServerConfig &server_config)
{
    // 構文チェック

    server_config.clearIndex();
    ++parse_line_word_;

   for (; parse_line_word_ != parse_line_->end(); ++parse_line_word_)
    {
        if (*parse_line_word_ == ";")
            break ;
        server_config.addIndex(*parse_line_word_);
    }
}

void ConfigParser::parseListenDirective(ServerConfig &server_config)
{
    // 構文チェック

    server_config.setListen(std::atoi(parse_line_word_[DIRECTIVE_VALUE].c_str()));
}

void ConfigParser::parseReturnDirective(ServerConfig &server_config)
{
    // 構文チェック

    server_config.addReturnRedirect(std::atoi(parse_line_word_[1].c_str()), parse_line_word_[2]);
}

void ConfigParser::parseServerNameDirective(ServerConfig &server_config)
{
    // 構文チェック

    server_config.setServerName(parse_line_word_[DIRECTIVE_VALUE]);
}

void ConfigParser::parseUploadPath(ServerConfig &server_config)
{
    // 構文チェック

    server_config.setUploadPath(parse_line_word_[DIRECTIVE_VALUE]);
}

bool ConfigParser::isServerContext(std::vector<std::vector<std::string> > ::const_iterator vviter)
{
    std::vector<std::string>::const_iterator viter = vviter->begin();

    if ((viter[DIRECTIVE_NAME] == "server")
        && (viter[SERVER_OPEN_BRACES] == "{")
        && (vviter->size() == 2))
    {
        return true;
    }
    return false;
}

// bool ConfigParser::isLocationContext(std::vector<std::vector<std::string> > ::const_iterator vviter)
// {
//     std::vector<std::string>::const_iterator viter = vviter->begin();

//     if ((config_file_.at(line_num).at(0) == "location")
//               && (config_file_.at(line_num).at(2) == "{")
//               && (config_file_.at(line_num).size() == 3))
//     if (*viter == "location"
//         && *(++viter) == "{")
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
