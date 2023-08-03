NAME = webserv
CC = clang++ -g -fsanitize=address 
FLAGS = -Wall -Wextra -Werror -Iincludes -std=c++98

SRCS =	main.cpp srcs/socket.cpp srcs/socket_utils.cpp srcs/request/request.cpp \
		srcs/matching.cpp srcs/cgi.cpp \
		srcs/Server.cpp srcs/parsing/config.cpp srcs/parsing/utilities.cpp\
		srcs/response/response.cpp srcs/response/response_utils.cpp\

all: webserv
$(NAME): $(SRCS) 
	 @$(CC) $(FLAGS) $(SRCS) -o webserv 
	 @echo "\033[0;32m®webserv CREATED 🐲\033[0m"
clean:
	@rm -rf webserv.o
	@echo "cleaned 🗑"
fclean : clean
		@rm -rf ./webserv webserv.dSYM
		@echo "fcleaned 🗑"
re : fclean all
