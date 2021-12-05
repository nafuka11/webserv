#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include "LocationConfig.hpp"
#include <map>
#include <string>

class ServerConfig
{
public:
	ServerConfig();
	~ServerConfig();

	void	setListen(const int port); // publicの必要はない！

	const int	listen() const;
	const std::string	serverName() const;
	const std::vector<std::string> allowMethods() const;
	const std::string cgiExtension() const;
	const int	clientMaxBodySize() const;
	const std::map<int, std::string> errorPage() const;
	const std::string uplocadPath() const;
	const std::map<std::string, LocationConfig> location();
private:
	int listen_;
	std::string server_name_;
	std::vector<std::string> allow_methods_;
	std::string cgi_extension_;
	int client_max_body_size_;
	std::map<int, std::string> error_page_;
	std::string upload_path;
	std::map<std::string, LocationConfig> location_;
};

#endif /* SERVERCONFIG_HPP */
