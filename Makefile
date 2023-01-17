# ----------- EXECUTABLE -----------

NAME		=	megaphone
DESCRIPTION =	Piscine CPP, Day 00 - ex00

# ----------- COMPILER FLAGS -------

CC			=	c++
CFLAGS		+=	-Wall -Wextra -Werror -g3 -std=c++98

# ----------- FILES ----------------

SRCS		=	./SRCS
OBJ			=	./objs
SRCS		=	
OBJS		=	$(patsubst $(SRC)/%.cpp, $(OBJS)/%.o,$(SRCS))

# ----------- COLORS ---------------

BLACK		= \033[1;30m
RED			= \033[1;31m
GREEN		= \033[1;32m
PURPLE		= \033[1;35m
CYAN		= \033[1;36m
WHITE		= \033[1;37m
BLUE		= \033[0;34m
EOC			= \033[0;0m

# ----------- RULES ----------------

all			: $(NAME)

${NAME}		: $(OBJS)
	@echo "$(BLUE) =========> Compiling object files <========="
	@echo "$(WHITE)"
		$(CC) $(CFLAGS) -o $(NAME) $(OBJS) 
	@echo "$(BLUE) =====> Build $(DESCRIPTION) DONE √ <====="
	@echo -n "$(EOC)"

%.o: %.cpp
	@echo -n "$(PURPLE)"
		mkdir -p ./objs/
		$(CC) $(CFLAGS) -c $< -o $@

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

