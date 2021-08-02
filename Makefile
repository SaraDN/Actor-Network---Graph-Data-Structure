all: build

build: movies.c
	gcc -Wall -g3 -std=c99 movies.c -o movies

clean:
	rm movies