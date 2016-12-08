all: manager router

manager: manager.o
	g++ -Wall -std=c++11 -I. manager.o -o manager -lpthread

manager.o: manager.cpp project3.h
	g++ -Wall -std=c++11 -I. -c manager.cpp -lpthread
	
router: router.o
	g++ -Wall -std=c++11 -I. router.o -o router -lpthread

router.o: router.cpp project3.h
	g++ -Wall -std=c++11 -I. -c router.cpp -lpthread

clean:
	rm -f *.o manager router

tar:
	tar -cvf P3.tar *.cpp *.h Makefile
