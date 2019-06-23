#!/bin/bash

if [[ $# -ne 2 ]];
then
	echo "Usage: build-library <clang/gcc> <dynamic/static>"
	exit 1
fi

if [[ $1 != "clang" && $1 != "gcc" ]];
then
	echo "Must either be clang or gcc"
	exit 1
fi

if [[ $2 == "dynamic" ]];
then
	cd src
	"$1" -shared -o containers.so -O3 -fPIC *.c
	cd ..
	mv src/containers.so containers.so
	echo "Now, you can copy-paste containers.h and containers.so to any project that you would like to use the dynamic library with."
	echo "Afterwards, your project can be compiled with: $1 test.c -o test containers.so -ldl"
elif [[ $2 == "static" ]];
then
	cd src
	"$1" *.c -c -O3 -fpic
	ar rcs containers.a *.o
	rm *.o
	cd ..
	mv src/containers.a containers.a
	echo "Now, you can copy-paste containers.h and containers.a to any project that you would like to use the static library with."
	echo "Afterwards, your project can be compiled with: $1 test.c -o test containers.a -ldl"
else
	echo "Must either be dynamic or static"
	exit 1
fi
