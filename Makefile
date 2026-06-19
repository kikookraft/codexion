NAME = codexion

CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread
INCLUDES = -Isrc/include
HEADER   = src/include/inc.h

SRC = \
	src/codexion.c \
	src/coders/parse.c \
	src/coders/routine.c \
	src/coders/action.c \
	src/coders/burnout.c \
	src/utils/time.c \
	src/utils/init.c \
	src/utils/queue.c \
	src/utils/print.c \
	src/simulation.c

OBJ = $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) $(INCLUDES) -o $(NAME)

%.o: %.c $(HEADER)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

run: re
	./$(NAME) 2 20 0 0 0 1 0 edf
.PHONY: all clean fclean re