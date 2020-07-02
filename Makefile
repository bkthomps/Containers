.DEFAULT_GOAL := static

static:
	clang src/*.c -c -O3 -fpic
	ar rcs containers.a *.o
	rm *.o

dynamic:
	clang -shared -o containers.so -O3 -fPIC src/*.c

clean:
	rm -f containers.a
	rm -f containers.so
