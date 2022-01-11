#ifndef CONFIGCONSTANT_HPP
#define CONFIGCONSTANT_HPP

#include <string>
#include <vector>

class ConfigConstant
{
public:
    static const int UNSET_TYPE_INT;
    static const std::string UNSET_TYPE_STR;

    static const std::vector<std::string> DEFAULT_ALLOW_METHOD;
    static const std::string DEFAULT_AUTOINDEX;
    static const int DEFAULT_CLIENT_MAX_BODY_SIZE;
    static const std::vector<std::string> DEFAULT_INDEX;
    static const int DEFAULT_PORT;
};

#endif /* CONFIGCONSTANT_HPP */
