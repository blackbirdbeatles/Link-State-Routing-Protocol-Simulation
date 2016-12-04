all: manager

manager: Manager.o Router.o
	g++ -Wall -std=c++11 -I. Manager.o Router.o -o manager

Manager.o: manager.cpp project3.h
	g++ -Wall -std=c++11 -I. -c manager.cpp

Router.o: router.cpp project3.h
	g++ -Wall -std=c++11 -I. -c router.cpp

clean:
	rm -f *.o manager

tar:
	tar -cvf manager.tar *.cpp *.h Makefile

