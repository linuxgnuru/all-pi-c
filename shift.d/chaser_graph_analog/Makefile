PROGNAME = chaser_graph_analog

all: clean $(PROGNAME)

$(PROGNAME):$(PROGNAME).o
	gcc -O2 -Wall $(PROGNAME).c -o $(PROGNAME) -lwiringPi -lncurses

clean:
	rm -f *.o $(PROGNAME)

