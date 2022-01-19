NAME		:= a.out
INC_DIR		:= ../includes
SRCS		:= ../srcs/AddressInfoError.cpp ../srcs/ClientSocket.cpp ../srcs/Config.cpp ../srcs/ConfigConstant.cpp ../srcs/ConfigError.cpp ../srcs/ConfigParser.cpp ../srcs/HTTPParseException.cpp ../srcs/HTTPParser.cpp ../srcs/HTTPRequest.cpp ../srcs/HTTPResponse.cpp ../srcs/KqueuePoller.cpp ../srcs/LocationConfig.cpp ../srcs/MainConfig.cpp ../srcs/ServerConfig.cpp ../srcs/ServerSocket.cpp ../srcs/Socket.cpp ../srcs/SystemError.cpp ../srcs/Uri.cpp ../srcs/Webserv.cpp ./conf_main.cpp
OBJS		:= $(SRCS:.cpp=.o)

CXX			:= clang++
CXXFLAGS	:=  -Wall -Wextra -Werror -std=c++98 -I$(INC_DIR) -pedantic

.PHONY		: all
all			: $(NAME) ## Build executable

$(NAME)	: $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

.PHONY		: clean
clean		: ## Delete object files
	$(RM) $(OBJS)
	$(RM) $(FOBJS)

.PHONY		: fclean
fclean		: clean ## Delete executable and object files
	$(RM) $(NAME)
	$(RM) $(FNAME)

.PHONY		: re
re			: fclean all ## Rebuild executable

.PHONY		: srcs
srcs		: ## Update SRCS
	sed -i .bak -e "s#^\(SRCS.*= \).*#\1$$(find srcs -name '*.cpp' -type f | tr '\n' ' ')#" ./Makefile
