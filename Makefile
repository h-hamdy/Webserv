CC	=	g++

CFLAGS	=	-Wall -Wextra -Werror -std=c++98

NAME	=	webserv

SRC	=	main.cpp \
		parsing/parse.cpp \
		parsing/parse_utils.cpp \
		parsing/lexer.cpp \
		parsing/lexer_utils.cpp \
		