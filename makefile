CC=g++
CC_PREFLAGS=-std=c++17
CC_POSTFLAGS=-static -static-libgcc -static-libstdc++
CC_FILE=maple-shell.cpp

ifeq ($(OS),Windows_NT)
	CC_FILE := platforms/mapleshell-win.cpp
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		CC_FILE := platforms/mapleshell-linux.cpp
	else
		ifeq ($(UNAME_S),Darwin)
			CC_FILE := platforms/mapleshell-mac.cpp
		endif
	endif
endif

output: maple-shell.o
	$(CC) maple-shell.o -o maple-shell $(CC_POSTFLAGS)

maple-shell.o: $(CC_FILE)
	$(CC) $(CC_PREFLAGS) -c $(CC_FILE) -o maple-shell.o
     
clean:
	rm *.o maple-shell