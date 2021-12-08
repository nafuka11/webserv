NAME		:= webserv

INC_DIR		:= ./includes
SRCS		:= srcs/ServerConfig.cpp srcs/ClientSocket.cpp srcs/SystemError.cpp srcs/LocationConfig.cpp srcs/Socket.cpp srcs/Webserv.cpp srcs/HTTPServer.cpp srcs/HTTPParser.cpp srcs/HTTPResponse.cpp srcs/Config.cpp srcs/AddressInfoError.cpp srcs/HTTPParseException.cpp srcs/main.cpp srcs/ServerSocket.cpp srcs/HTTPRequest.cpp
OBJS		:= $(SRCS:.cpp=.o)

CXX			:= clang++
CXXFLAGS	:= -Wall -Wextra -Werror -std=c++98 -I$(INC_DIR) -pedantic

.PHONY		: all
all			: $(NAME) ## Build executable

$(NAME)	: $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

.PHONY		: clean
clean		: ## Delete object files
	$(RM) $(OBJS)

.PHONY		: fclean
fclean		: clean ## Delete executable and object files
	$(RM) $(NAME)

.PHONY		: re
re			: fclean all ## Rebuild executable

.PHONY		: srcs
srcs		: ## Update SRCS
	sed -i .bak -e "s#^\(SRCS.*= \).*#\1$$(find srcs -name '*.cpp' -type f | tr '\n' ' ')#" ./Makefile

.PHONY		: help
help		: ## Show this message
	@echo "Target lists:"
	@grep -E '^[a-zA-Z_-]+\t*:.*?## .*$$' Makefile | awk 'BEGIN {FS = "\t*:.*?## "}; {printf "    \033[36m%-10s\033[0m %s\n", $$1, $$2}'
