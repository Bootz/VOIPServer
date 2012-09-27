##
## Makefile for openGL in /home/perely_k/work/openGL
## 
## Made by kyrylo perelygin
## Login   <perely_k@epitech.net>
## 
## Started on  Sun May  3 15:35:14 2009 kyrylo perelygin
## Last update Sun Nov 29 19:40:55 2009 kyrylo perelygin
##

CC	=	g++
SRC	=	main.cpp \
		CServer.cpp \
		CClient.cpp \
		CConfig.cpp \
		CRequest.cpp \
		CChannel.cpp \
		CRequestHandler.cpp \

OBJ	=	$(SRC:.cpp=.o)
NAME	=	Server
LIBS	=	-L$(HOME)/.boost/lib/ -lboost_system -lpthread
INCLUDE = 	-I$(HOME)/.boost/include/
CFLAGS	=	-W -Wall -Wno-deprecated -pedantic $(INCLUDE)
CONFIG	=	Release

$(NAME): $(OBJ)
	$(CC) -o $(NAME) $(OBJ) $(LIBS)
	mv $(NAME) $(CONFIG)/

all: $(NAME)

clean:
	rm -f $(OBJ)
	rm -f *~

fclean:	clean
	rm -f $(CONFIG)/$(NAME)

re: 	fclean all

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: all clean fclean re
