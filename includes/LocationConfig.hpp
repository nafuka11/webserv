#ifndef LOCATIONCONFIG_HPP
#define LOCATIONCONFIG_HPP

#include <map>
#include <string>
#include <vector>

class LocationConfig
{
public:
	LocationConfig();
	~LocationConfig();
	const std::string alias() const;
	const std::string autoindex() const;
	const std::vector<std::string> index() const;
	const std::map<int, std::string> returnRedirect() const;
private:
	std::string	alias_;
	std::string autoindex_;
	std::vector<std::string> index_;
	std::map<int, std::string> return_;
};

#endif /* LOCATIONCONFIG_HPP */