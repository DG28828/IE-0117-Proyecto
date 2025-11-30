CC      := gcc
CFLAGS  := -std=c11 -Wall -Wextra -Werror -pedantic -O2 -Wno-unused-parameter
TARGET  := visualzador

SRC     := src/main.c
OBJ     := $(SRC:.c=.o)

GTK_CFLAGS := $(shell pkg-config --cflags gtk+-3.0)
GTK_LIBS   := $(shell pkg-config --libs gtk+-3.0)

.PHONY: all run clean valgrind

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@ $(GTK_LIBS)

src/%.o: src/%.c
	$(CC) $(CFLAGS) $(GTK_CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

valgrind: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(TARGET)

deps:
	sudo apt-get install libgtk-3-dev gnuplot build-essential pkg-config valgrind

clean:
	rm -f $(OBJ) $(TARGET)



