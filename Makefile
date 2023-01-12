RESET="\033[0m"

BOLDBLACK="\033[1m\033[30m"
BOLDRED="\033[1m\033[31m"
BOLDGREEN="\033[1m\033[32m"
BOLDYELLOW="\033[1m\033[33m"
BOLDBLUE="\033[1m\033[34m"
BOLDMAGENTA="\033[1m\033[35m"
BOLDCYAN="\033[1m\033[36m"
BOLDWHITE="\033[1m\033[37m"

SRC = srcs/main.cpp\
		srcs/Utils.cpp\
		srcs/socket/server.cpp\
		srcs/socket/client.cpp\
		srcs/config/Config.cpp

NAME = webserv

INCLUDE = -I ./srcs/include -I ./srcs/config/include

DEP = ${SRC:.cpp=.d}

CC = c++

CFLAGS = -MMD -Wall -Wextra -Werror ${INCLUDE} -std=c++98 -g 

OBJ = $(SRC:.cpp=.o) 

all: ${NAME} 

%.o: %.cpp
	${CC} ${CFLAGS} -c $< -o ${<:.cpp=.o}

$(NAME):		$(OBJ)
				@${CC} ${CFLAGS}  -o ${NAME} ${OBJ}
				@echo ${BOLDGREEN}"          [OK]\n"${RESET}

-include $(DEP)
clean:
	rm -f ${OBJ}
	rm -f $(DEP)
	
fclean: clean
	rm -f ${NAME}

re : fclean all

.PHONY: clean re fclean all