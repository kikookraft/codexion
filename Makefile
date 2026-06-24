NAME = codexion

CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread
INCLUDES = -Isrc/include
HEADER   = src/include/inc.h

SRC = \
	src/codexion.c \
	src/parse.c \
	src/simulation.c \
	src/coders/routine.c \
	src/coders/action.c \
	src/coders/burnout.c \
	src/utils/time.c \
	src/utils/init.c \
	src/utils/init_dongle.c \
	src/utils/init_coders.c \
	src/utils/queue.c \
	src/utils/print.c \
	src/utils/log_suppress.c \
	src/utils/safe.c \
	src/dongle/dongle.c

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

.PHONY: all clean fclean re
