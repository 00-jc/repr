CC=cc 
SRC=repr.c
CFLAGS=-Wall -Wextra -Wstrict-aliasing -Wshadow -Wconversion -pedantic -Waddress -Walloca \
	   -Wambiguous-ellipsis -Wambiguous-macro -Wassume -Wattributes -Wpessimizing-move \
	   -Wbounds-safety-counted-by-elt-type-unknown-size -Wbuiltin-assume-aligned-alignment \
	   -Wgnu-union-cast -Winline -Wno-gnu-statement-expression-from-macro-expansion \
	   -Wno-language-extension-token -D_GNU_SOURCE -DNDEBUG -finline-functions \
	   -fvisibility=hidden -march=native -O3 -ffunction-sections -fdata-sections -fvectorize \
	   -flto=full -fno-plt -pipe
LDFLAGS=-Wl,-O3 -Wl,-s -Wl,-z,now -Wl,--gc-sections
NAME=repr

all:
	$(CC) -o $(NAME) $(SRC) $(CFLAGS) $(LDFLAGS)
clean:
	rm $(NAME)

.PHONY: all clean
