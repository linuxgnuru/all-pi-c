PROGNAME = graph
CC = gcc
#CFLAGS = -Wall -O2 -std=c11
CFLAGS = -Wall -O2
LIBS = -lwiringPi
SRC = $(PROGNAME).c
OBJ = $(SRC:.c=.o)

ifeq ($(PREFIX),)
	PREFIX := /usr/local
endif

all: clean $(PROGNAME)

$(PROGNAME): $(OBJ)
	@$(CC) -o $@ $(OBJ) $(LIBS)

.c.o:
	@$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJ) $(PROGNAME)

install: $(PROGNAME)
	sudo install -d $(PREFIX)/bin/
	sudo install -m 4755 $(PROGNAME) $(PREFIX)/bin/

