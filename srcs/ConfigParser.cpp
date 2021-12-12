#include "ConfigParser.hpp"
#include <fstream>
#include <iostream>//del;
#include <string>
#include "Config.hpp"
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
    "allow_methods",
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
    parseLines();
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

void ConfigParser::parseLines()
{
    MainConfig main_config = MainConfig();

    for (parse_line_ = config_file_.begin();
         parse_line_ != config_file_.end();
         ++parse_line_)
    {
        if (parse_line_->size() == 0) // SEGV on unknown addressを回避するため（parseMainContextで要素[0]を参照した時に発生）
            continue;
        parse_line_word_ = parse_line_->begin();
        parseMainContext(main_config);
        ++num_line_;
    }
}

void ConfigParser::parseMainContext(MainConfig &main_config)
{
    void (ConfigParser::*main_directive_func[NUM_MAIN_DIRECTIVE])(MainConfig&) = {
        &ConfigParser::parseAllowMethodDirective,
        &ConfigParser::parseAutoindexDirective,
        &ConfigParser::parseCgiExtensionDirective,
        &ConfigParser::parseClientMaxBodySizeDirective,
        &ConfigParser::parseErrorPageDirective,
        &ConfigParser::parseIndexDirective,
        &ConfigParser::parseServerContext
    };

    for (int i = 0; i < NUM_MAIN_DIRECTIVE; ++i)
    {
        if (parse_line_word_[DIRECTIVE_NAME] == MAIN_DIRECTIVE[i])
        {
            (this->*main_directive_func[i])(main_config);
        }
    }
}

void ConfigParser::parseServerContext(MainConfig &main_config)
{
    std::cout << "Line." << num_line_ << " server" << std::endl; // 後で消す

    ServerConfig server = ServerConfig(main_config);

    config_.addServer(server);
}

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
    std::cout << "Line." << num_line_ << " autoindex" << std::endl; // 後で消す

    //構文チェック

    main_config.setAutoIndex(parse_line_word_[1]);
}

void ConfigParser::parseCgiExtensionDirective(MainConfig &main_config)
{
    std::cout << "Line." << num_line_ << " cgi_extension" << std::endl; // 後で消す

    //構文チェック
    main_config.setcgiExtension(parse_line_word_[1]);
}

void ConfigParser::parseClientMaxBodySizeDirective(MainConfig &main_config)
{
    std::cout << "Line." << num_line_ << " client_max_body_size" << std::endl; // 後で消す

    //構文チェック
    main_config.setClientMaxBodySize(std::atoi(parse_line_word_[1].c_str()));
}

void ConfigParser::parseErrorPageDirective(MainConfig &main_config)
{
    std::cout << "Line." << num_line_ << " error_page"  << std::endl; // 後で消す

    //構文チェック

    main_config.clearErrorPage();

    // ex) error_page 404 /404.html;
    if (parse_line_->size() == 4)
    {
        main_config.addErrorPage(std::atoi(parse_line_word_[1].c_str()), parse_line_word_[2]);

        return ;
    }
    // ex) error_page 500 502 503 504 /50x.html;
    std::vector<std::string>::const_iterator status_code = parse_line_->begin();
    ++status_code;

    std::vector<std::string>::const_iterator uri = parse_line_->end();
    uri = uri - 2;

    for (; status_code != uri; ++status_code)
    {
        main_config.addErrorPage(std::atoi(status_code->c_str()), *uri);
    }
}

void ConfigParser::parseIndexDirective(MainConfig &main_config)
{
    std::cout << "Line." << num_line_ << " index" << std::endl; // 後で消す

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
