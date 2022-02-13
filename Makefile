NAME		:= webserv

INC			:= -I ./srcs/config/ -I ./srcs/exception/ -I ./srcs/server/
SRCS		:= srcs/config/Config.cpp srcs/config/ConfigConstant.cpp srcs/config/ConfigError.cpp srcs/config/ConfigParser.cpp srcs/config/LocationConfig.cpp srcs/config/MainConfig.cpp srcs/config/ServerConfig.cpp srcs/exception/AddressInfoError.cpp srcs/exception/HTTPParseException.cpp srcs/exception/SystemError.cpp srcs/main.cpp srcs/server/CGI.cpp srcs/server/CGIParser.cpp srcs/server/ClientSocket.cpp srcs/server/HTTPParser.cpp srcs/server/HTTPRequest.cpp srcs/server/HTTPResponse.cpp srcs/server/KqueuePoller.cpp srcs/server/ServerSocket.cpp srcs/server/Socket.cpp srcs/server/Uri.cpp srcs/server/Webserv.cpp
OBJS		:= $(SRCS:.cpp=.o)
DEPS		:= $(SRCS:.cpp=.d)

CXX			:= clang++
CXXFLAGS	:= -Wall -Wextra -Werror -std=c++98 $(INC) -pedantic -MMD -MP

.PHONY		: all
all			: $(NAME) ## Build executable

$(NAME)	: $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

.PHONY		: clean
clean		: ## Delete object files and dependency files
	$(RM) $(OBJS) $(DEPS)

.PHONY		: fclean
fclean		: clean ## Delete executable, object files and dependency files
	$(RM) $(NAME)

.PHONY		: re
re			: fclean all ## Rebuild executable

.PHONY		: srcs
srcs		: ## Update SRCS
	sed -i .bak -e "s#^\(SRCS.*= \).*#\1$$(find srcs -name '*.cpp' -type f | sort | tr '\n' ' ' | sed 's/ *$$//')#" ./Makefile

.PHONY		: debug
debug		: clean ## Shows server settings
	$(MAKE) CXXFLAGS="$(CXXFLAGS) -D DEBUG"
	$(MAKE) clean

.PHONY		: help
help		: ## Show this message
	@echo "Target lists:"
	@grep -E '^[a-zA-Z_-]+\t*:.*?## .*$$' Makefile | awk 'BEGIN {FS = "\t*:.*?## "}; {printf "    \033[36m%-10s\033[0m %s\n", $$1, $$2}'

-include $(DEPS)
