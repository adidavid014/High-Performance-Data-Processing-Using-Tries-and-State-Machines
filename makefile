all: standard optimized

standard: standard.o
	g++ -Wall -Wextra -pedantic standard.o -o standard

standard.o: standard.cpp
	g++ -Wall -Wextra -pedantic -c standard.cpp

optimized: optimized.o
	g++ -Wall -Wextra -pedantic optimized.o -o optimized

optimized.o: optimized.cpp
	g++ -Wall -Wextra -pedantic -c optimized.cpp

clean: 
	rm -f *.o standard optimized
