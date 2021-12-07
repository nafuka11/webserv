NAME		:= a.out
INC_DIR		:= ../includes
SRCS		:= conf_main.cpp ../srcs/ConfigParser.cpp ../srcs/Config.cpp ../srcs/ServerConfig.cpp ../srcs/LocationConfig.cpp
OBJS		:= $(SRCS:.cpp=.o)

FNAME		:= file
FINC_DIR	:= ../includes
FSRCS		:= file_main.cpp ../srcs/ConfigParser.cpp ../srcs/Config.cpp ../srcs/ServerConfig.cpp ../srcs/LocationConfig.cpp
FOBJS		:= $(FSRCS:.cpp=.o)

CXX			:= clang++
CXXFLAGS	:= -Wall -Wextra -Werror -std=c++98 -I$(INC_DIR) -pedantic

.PHONY		: all
all			: $(NAME) ## Build executable

$(NAME)	: $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

.PHONY		: file
file		: $(FNAME)
$(FNAME) : $(FOBJS)
	$(CXX) $(CXXFLAGS) -o $(FNAME) $(FOBJS)

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
