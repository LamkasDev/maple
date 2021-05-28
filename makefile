CC=g++
CC_PREFLAGS=-std=c++17
CC_POSTFLAGS=-static -static-libgcc -static-libstdc++

output: maple-shell.o
	$(CC) maple-shell.o -o maple-shell $(CC_POSTFLAGS)

maple-shell.o: maple-shell.cpp
	$(CC) $(CC_PREFLAGS) -c maple-shell.cpp
     
clean:
	rm *.o maple-shell