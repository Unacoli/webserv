# ----------- EXECUTABLE -----------

NAME		=	webserv
DESCRIPTION =	This is when you finally understand why a URL starts with HTTP

# ----------- COMPILER FLAGS -------

CC			=	c++
CFLAGS		+=	-Wall -Wextra -Werror -g3 -std=c++98

# ----------- INCLUDE --------------

INCLUDE		= ./include/

# ----------- FILES ----------------

SRC			=	./srcs
OBJ			=	./objs
SRCS		=	$(SRC)/main.cpp			\
				$(SRC)/Cgi.cpp			\
				$(SRC)/RequestHTTP.cpp	\
				$(SRC)/ResponseHTTP.cpp	\
				$(SRC)/client.cpp		\
				$(SRC)/Config.cpp		\
				$(SRC)/WebServer.cpp		\
				$(SRC)/utils.cpp		
OBJS		=	$(patsubst $(SRC)/%.cpp, $(OBJ)/%.o,$(SRCS))

# ----------- COLORS ---------------

all: ${NAME}
	

# ----------- RULES ----------------

all			: $(NAME)

${NAME}		: $(OBJS)
		
	@echo "$(BLUE) =========> Compiling object files <========="
	@echo "$(WHITE)"
		$(CC) $(CFLAGS) -o $(NAME) $(OBJS) 
	@echo "$(BLUE) =====> Build $(DESCRIPTION) DONE √ <====="
	@echo -n "$(EOC)"

$(OBJ)/%.o: $(SRC)/%.cpp
	@echo -n "$(PURPLE)"
		touch example-website/poke-website/data/data
		mkdir -p ./objs/
		$(CC) $(CFLAGS) -I $(INCLUDE) -c $< -o $@

clean		:
	@echo "$(BLUE) =========> Deleting object files <========="
	@echo "$(PURPLE)"
		$(RM) $(OBJS)

fclean		: clean
	@echo "$(BLUE) =========> Deleting executable <========="
	@echo "$(PURPLE)"
		$(RM) $(NAME)

re			: fclean all

.PHONY: all clean fclean re