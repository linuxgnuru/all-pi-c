PROGNAME = everything

all: clean $(PROGNAME)

everything:
	cmake -H. -Bbuild
	cmake --build build --

clean:
	rm -rf bin build
