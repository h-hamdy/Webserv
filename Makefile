# makefile that compile all cpp files in the current directory
# and creates a binary file named "main"

CC = clang++ -g -fsanitize=address 
FLAGS = -Wall -Wextra -Werror -Iincludes -std=c++98

<<<<<<< HEAD
SRCS =	main.cpp srcs/socket.cpp srcs/request/request.cpp srcs/cgi.cpp \
		srcs/Server.cpp srcs/parsing/config.cpp srcs/parsing/utilities.cpp srcs/matching.cpp \
		srcs/response/response.cpp  \
# srcs/res/res-GET.cpp 
=======
SRCS =	main.cpp srcs/socket.cpp srcs/socket_utils.cpp srcs/request/request.cpp \
		srcs/matching.cpp srcs/cgi.cpp \
		srcs/Server.cpp srcs/parsing/config.cpp srcs/parsing/utilities.cpp\
		srcs/response/response.cpp\
		# srcs/res/res-GET.cpp 
>>>>>>> d010e01411ae7005e3d751558cfc695df1bc0fbd
# CONFIG = 

all: webserv

# webserv: $(SRCS) $(CONFIG) # $(CC) $(FLAGS) $(SRCS) $(CONFIG) -o webserv 
webserv: $(SRCS) 
	 @$(CC) $(FLAGS) $(SRCS) -o webserv 
	 @echo "\033[0;32m®webserv CREATED 🐲\033[0m"
clean:
	@rm -rf webserv.o
	@echo "cleaned 🗑"
fclean : clean
		@rm -rf ./webserv webserv.dSYM
		@echo "fcleaned 🗑"
re : fclean all
