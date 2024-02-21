all: prj2

prj2: prj2.o
	g++ -Wall -Wextra -pedantic prj2.o -o prj2
	
prj2.o: prj2.cpp
	g++ -Wall -Wextra -pedantic -c prj2.cpp

clean: 
	rm -f *.o prj2
