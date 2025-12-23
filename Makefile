NAME = libcutil.a

LIBS = 
CFLAGS = -Wall -Wextra
CFLAGS += $(pkg-config --cflags $(LIBS))
LDFLAGS = $(pkg-config --libs $(LIBS))

SRC = $(wildcard file/*.c)
OBJ = $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	ar rcs $(NAME) $(OBJ)

prod: CFLAGS += -DPROD
prod: re

clean:
	rm -f $(OBJ) $(NAME)

re: clean all

.PHONY: all prod clean re
