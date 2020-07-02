.DEFAULT_GOAL := unspecified

unspecified:
	@echo "Error: use 'make static_clang' or 'make dynamic_clang' or 'make static_gcc' or 'make dynamic_gcc'"

static_clang:
	clang src/*.c -c -O3 -fpic
	ar rcs containers.a *.o
	rm *.o

dynamic_clang:
	clang -shared -o containers.so -O3 -fPIC src/*.c

static_gcc:
	gcc src/*.c -c -O3 -fpic
	ar rcs containers.a *.o
	rm *.o

dynamic_gcc:
	gcc -shared -o containers.so -O3 -fPIC src/*.c

clean:
	rm -f containers.a
	rm -f containers.so
