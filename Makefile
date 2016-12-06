all: manager router

manager: manager.o
	g++ -Wall -std=c++11 -I. manager.o -o manager -pthread

manager.o: manager.cpp project3.h
	g++ -Wall -std=c++11 -I. -c manager.cpp -pthread
	
router: router.o
	g++ -Wall -std=c++11 -I. router.o -o router

router.o: router.cpp project3.h
	g++ -Wall -std=c++11 -I. -c router.cpp

clean:
	rm -f *.o manager router

tar:
	tar -cvf P3.tar *.cpp *.h Makefile
