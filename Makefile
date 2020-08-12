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

header:
	python3 compile_headers.py $(version)

test_debug:
	@clang src/*.c tst/*.c -Wall -Wextra -Wpedantic -Werror -std=c89 -O0 -ldl -o ContainersTestDebug

test_optimized:
	@clang src/*.c tst/*.c -Wall -Wextra -Wpedantic -Werror -std=c89 -O3 -ldl -o ContainersTestOptimized

test_coverage:
	@clang src/*.c tst/*.c -Wall -Wextra -Wpedantic -std=c89 -O0 -ldl -g -fprofile-arcs -ftest-coverage -o ContainersTestCoverage

valgrind:
	find tst/test.h -type f -exec sed -i "" "s/STUB_MALLOC 1/STUB_MALLOC 0/g" {} \;
	@clang src/*.c tst/*.c -Wall -Wextra -Wpedantic -Werror -std=c89 -O0 -ldl -o ContainersTestValgrind
	find tst/test.h -type f -exec sed -i "" "s/STUB_MALLOC 0/STUB_MALLOC 1/g" {} \;
	@valgrind --leak-check=full ./ContainersTestValgrind
