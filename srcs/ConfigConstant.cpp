#include "ConfigConstant.hpp"

// Unset Directive Value
const int ConfigConstant::UNSET_TYPE_INT = -1;
const std::string ConfigConstant::UNSET_TYPE_STR = "";

// Default Directive Value
std::string default_allow_method[] = {"GET", "POST", "DELETE"};
std::string default_index[] = {"index.html"};

const std::vector<std::string>
    ConfigConstant::DEFAULT_ALLOW_METHOD(default_allow_method, std::end(default_allow_method));
const std::string ConfigConstant::DEFAULT_AUTOINDEX = "off";
const size_t ConfigConstant::DEFAULT_CLIENT_MAX_BODY_SIZE = 5000;
const std::vector<std::string>
    ConfigConstant::DEFAULT_INDEX(default_index, std::end(default_index));
const size_t ConfigConstant::DEFAULT_PORT = 80;
