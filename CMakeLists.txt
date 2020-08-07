cmake_minimum_required(VERSION 3.5.1)
project(Containers C)

set(CMAKE_C_STANDARD 90)

set(CMAKE_C_FLAGS "-Wall -Wextra -Wpedantic -ldl -g -O0 -fprofile-arcs -ftest-coverage")

add_executable(Containers tst/test.c tst/test.h
        src/array.c src/include/array.h tst/array.c
        src/vector.c src/include/vector.h tst/vector.c
        src/deque.c src/include/deque.h tst/deque.c
        src/forward_list.c src/include/forward_list.h tst/forward_list.c
        src/list.c src/include/list.h tst/list.c
        src/set.c src/include/set.h tst/set.c
        src/map.c src/include/map.h tst/map.c
        src/multiset.c src/include/multiset.h tst/multiset.c
        src/multimap.c src/include/multimap.h tst/multimap.c
        src/unordered_set.c src/include/unordered_set.h tst/unordered_set.c
        src/unordered_map.c src/include/unordered_map.h tst/unordered_map.c
        src/unordered_multiset.c src/include/unordered_multiset.h tst/unordered_multiset.c
        src/unordered_multimap.c src/include/unordered_multimap.h tst/unordered_multimap.c
        src/stack.c src/include/stack.h tst/stack.c
        src/queue.c src/include/queue.h tst/queue.c
        src/priority_queue.c src/include/priority_queue.h tst/priority_queue.c)
