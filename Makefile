
mem:	gpiomem.cpp
	g++ -g -O1 -o mem gpiomem.cpp -Wall -std=gnu++17

clean:
	rm -f mem gpiomem.o core
