CC=g++
CC_DEBUG_PREFLAGS=-std=c++17 -I "C:\Program Files\maple_devtools-win\OpenSSL\include"
CC_PREFLAGS=-std=c++17 -Os -flto -fdata-sections -ffunction-sections
CC_POSTFLAGS=
CC_FILE=maple-shell.cpp

ifeq ($(OS),Windows_NT)
	CC_PREFLAGS=-std=c++17 -O2 -flto -fdata-sections -ffunction-sections -I "C:\Program Files\maple_devtools-win\OpenSSL\include"
	CC_POSTFLAGS=-Wl,--gc-sections -L "C:\Program Files\maple_devtools-win\OpenSSL\lib" -Wl,-Bstatic -lssl -lcrypto -Wl,-Bdynamic -lcrypt32 -lgdi32 -lws2_32 -lz
	CC_FILE := platforms/mapleshell-win.cpp
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		CC_PREFLAGS=-std=c++17 -Os -flto -fdata-sections -ffunction-sections -I "/usr/include/openssl"
		CC_POSTFLAGS=-Wl,--gc-sections -L "usr/lib/openssl" -Wl,-Bstatic -lssl -lcrypto -Wl,-Bdynamic -lpthread -ldl -lz
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
	$(CC) $(CC_DEBUG_PREFLAGS) -ggdb -c $(CC_FILE) -o maple-shell.o
	$(CC) maple-shell.o -o maple-shell $(CC_POSTFLAGS)

clean:
	rm *.o maple-shell

FORCE: ;
