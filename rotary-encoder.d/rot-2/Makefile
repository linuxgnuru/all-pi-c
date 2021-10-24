PROGNAME = rot-2
#INCLUDE = -IINCLUDES
DEBUG = -O2 -ggdb
#CFLAGS = $(DEBUG) -Wall $(INCLUDE)
CFLAGS = $(DEBUG) -Wall
LIBS = -lwiringPi -lncurses
#LIBS = -lwiringPi -lpthread
CC = gcc
SRC = main.c rotary-encoder.c
OBJ =   $(SRC:.c=.o)

ifeq ($(PREFIX),)
	PREFIX := /usr/local
endif

all: clean $(PROGNAME)

$(PROGNAME): $(OBJ)
	@$(CC) -o $@ $(OBJ) $(LIBS)

.c.o:
	@$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f *.o $(PROGNAME)

install: $(PROGNAME)
	sudo install -d $(PREFIX)/bin/
	sudo install -m 4755 $(PROGNAME) $(PREFIX)/bin/

