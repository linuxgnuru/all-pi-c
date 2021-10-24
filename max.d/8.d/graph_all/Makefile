PROGNAME = graph_all
DEBUG = -O2 -ggdb
CFLAGS = $(DEBUG) -Wall -std=c++11
LIBS = -lwiringPi
CC = g++
SRC = $(PROGNAME).cpp RasPiSPI.cpp
OBJ =   $(SRC:.cpp=.o)

ifeq ($(PREFIX),)
	PREFIX := /usr/local
endif

all: clean $(PROGNAME)

$(PROGNAME): $(OBJ)
	@$(CC) -o $@ $(OBJ) $(LIBS)

.cpp.o:
	@$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f *.o $(PROGNAME)

install: $(PROGNAME)
	sudo install -d $(PREFIX)/bin/
	sudo install -m 4755 $(PROGNAME) $(PREFIX)/bin/

