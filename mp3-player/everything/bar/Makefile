PROGNAME = bar
DEBUG = -O2 -ggdb
CFLAGS = $(DEBUG) -Wall -std=c11
LIBS = -lwiringPi
CC = gcc
SRC = $(PROGNAME).c
OBJ =   $(SRC:.c=.o)

ifeq ($(PREFIX),)
	PREFIX := /usr/local
endif

all: clean $(PROGNAME)

$(PROGNAME): $(OBJ)
	@$(CC) -o $@ $(OBJ) $(LIBS)
	rm -f *.o

.c.o:
	@$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f *.o $(PROGNAME)

install: $(PROGNAME)
	sudo install -d $(PREFIX)/bin/
	sudo install -m 4755 $(PROGNAME) $(PREFIX)/bin/

