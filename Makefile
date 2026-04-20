SRC_FILES=codexion
SRC_TARGETS=$(addsuffix .o, $(SRC_FILES))
CFLAGS=-Wall -Wextra -Werror -pthread

HEADERS=coders/inc.h
HEADERS_DIRS=-I coders -I headers

NAME=codexion

$(NAME): $(SRC_TARGETS) $(CODERS_OBJ)
$(NAME): $(SRC_TARGETS)
	cc $(CFLAGS) $(SRC_TARGETS) $(HEADERS_DIRS) -o $@

%.o: %.c $(HEADERS)
	cc -c $(CFLAGS) $(HEADERS_DIRS) $< -o $@

all: $(NAME)

clean:
	rm -f $(SRC_TARGETS)

fclean: clean
	rm -f $(NAME)

re: fclean all

run: $(NAME)
	./$(NAME) 1 2 3 4 5 6 7 edf

.PHONY: all clean fclean re run

.PHONY: all clean fclean re run