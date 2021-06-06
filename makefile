CC=g++
CC_PREFLAGS=-std=c++17 -I "C:\Program Files\OpenSSL-Win64\include"
CC_POSTFLAGS=-static -static-libgcc -static-libstdc++
CC_FILE=maple-shell.cpp

ifeq ($(OS),Windows_NT)
	CC_PREFLAGS=-std=c++17 -I "C:\Program Files\OpenSSL-Win64\include"
	CC_POSTFLAGS=-L "C:\Program Files\OpenSSL-Win64\lib" -static -static-libgcc -static-libstdc++ -lssl -lcrypto -lcrypt32 -lgdi32 -lws2_32
	CC_FILE := platforms/mapleshell-win.cpp
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		CC_PREFLAGS=-std=c++17 -I "/usr/include/openssl"
		CC_POSTFLAGS=-static -static-libgcc -static-libstdc++ -lssl -lcrypto -lpthread -ldl
		CC_FILE := platforms/mapleshell-linux.cpp
	else
		ifeq ($(UNAME_S),Darwin)
			CC_FILE := platforms/mapleshell-mac.cpp
		endif
	endif
endif

maple-shell: FORCE
	$(CC) $(CC_PREFLAGS) -c $(CC_FILE) -o maple-shell.o
	$(CC) maple-shell.o -o maple-shell $(CC_POSTFLAGS)
		
debug:
	$(CC) $(CC_PREFLAGS) -ggdb -c $(CC_FILE) -o maple-shell.o
	$(CC) maple-shell.o -o maple-shell $(CC_POSTFLAGS)

clean:
	rm *.o maple-shell

FORCE: ;
