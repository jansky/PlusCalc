all: libpluscalc pluscalcint

libpluscalc: pluscalc.o parser.o
	ar rcs libpluscalc.a pluscalc.o parser.o

pluscalcint: libpluscalc pluscalcint.cpp
	g++ -std=c++0x -g -o pluscalcint pluscalcint.cpp libpluscalc.a -lreadline

pluscalc.o: pluscalc.cpp
	g++ -std=c++0x -g -c pluscalc.cpp

parser.o: parser.cpp
	g++ -std=c++0x -g -c parser.cpp

install:
	cp pluscalcint /usr/bin
	cp libpluscalc.a /usr/lib
	cp pluscalc.h /usr/include

uninstall:
	rm /usr/bin/pluscalcint
	rm /usr/lib/libpluscalc.a
	rm /usr/include/pluscalc.h

clean:
	rm *.o
	rm *.a
	rm pluscalcint
