cmake_minimum_required(VERSION 3.5.1)
project(Containers C)

set(CMAKE_C_STANDARD 11)

set(CMAKE_C_FLAGS "-g -O0 -Wall -fprofile-arcs -ftest-coverage")
set(CMAKE_C_OUTPUT_EXTENSION_REPLACE 1)

add_executable(Containers tst/test.c tst/test.h
        src/array.c src/array.h tst/array.c
        src/vector.c src/vector.h tst/vector.c
        src/deque.c src/deque.h tst/deque.c
        src/forward_list.c src/forward_list.h tst/forward_list.c
        src/list.c src/list.h tst/list.c
        src/set.c src/set.h tst/set.c
        src/map.c src/map.h tst/map.c
        src/multiset.c src/multiset.h tst/multiset.c
        src/multimap.c src/multimap.h tst/multimap.c
        src/unordered_set.c src/unordered_set.h tst/unordered_set.c
        src/unordered_map.c src/unordered_map.h tst/unordered_map.c
        src/unordered_multiset.c src/unordered_multiset.h tst/unordered_multiset.c
        src/unordered_multimap.c src/unordered_multimap.h tst/unordered_multimap.c
        src/stack.c src/stack.h tst/stack.c
        src/queue.c src/queue.h tst/queue.c
        src/priority_queue.c src/priority_queue.h tst/priority_queue.c)
