NAME		:= webserv

INC_DIR		:= ./includes
SRCS		:= srcs/AddressInfoError.cpp srcs/ClientSocket.cpp srcs/Config.cpp srcs/ConfigConstant.cpp srcs/ConfigError.cpp srcs/ConfigParser.cpp srcs/HTTPParseException.cpp srcs/HTTPParser.cpp srcs/HTTPRequest.cpp srcs/HTTPResponse.cpp srcs/KqueuePoller.cpp srcs/LocationConfig.cpp srcs/MainConfig.cpp srcs/ServerConfig.cpp srcs/ServerSocket.cpp srcs/Socket.cpp srcs/SystemError.cpp srcs/Uri.cpp srcs/Webserv.cpp srcs/main.cpp
OBJS		:= $(SRCS:.cpp=.o)
DEPS		:= $(SRCS:.cpp=.d)

CXX			:= clang++
CXXFLAGS	:= -Wall -Wextra -Werror -std=c++98 -I$(INC_DIR) -pedantic -MMD -MP

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

.PHONY		: help
help		: ## Show this message
	@echo "Target lists:"
	@grep -E '^[a-zA-Z_-]+\t*:.*?## .*$$' Makefile | awk 'BEGIN {FS = "\t*:.*?## "}; {printf "    \033[36m%-10s\033[0m %s\n", $$1, $$2}'

-include $(DEPS)
