CC=g++
CFLAGS=-static-libgcc -static-libstdc++

output: maple-shell.o
	$(CC) maple-shell.o -o maple-shell $(CFLAGS)

maple-shell.o: maple-shell.cpp
	$(CC) -c maple-shell.cpp
     
clean:
	rm *.o maple-shell