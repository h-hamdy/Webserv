# makefile that compile all cpp files in the current directory
# and creates a binary file named "main"

CC =clang++ -g -fsanitize=address
CFLAGS = -Wall -Wextra -Werror -g
CFLAGS += -std=c++98

SRCS = socket.cpp main.cpp
# CONFIG = 

all: webserv

# webserv: $(SRCS) $(CONFIG) # $(CC) $(CFLAGS) $(SRCS) $(CONFIG) -o webserv 
webserv: $(SRCS) 
	 @$(CC) $(CFLAGS) $(SRCS) -o webserv 
	 @echo "\033[0;32m®webserv CREATED 🐲\033[0m"
clean:
	@rm -rf webserv.o
	@echo "cleaned 🗑"
fclean : clean
		@rm -rf ./webserv webserv.dSYM 
		@echo "fcleaned 🗑"
re : fclean all
