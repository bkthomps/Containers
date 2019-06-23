[![CircleCI](https://circleci.com/gh/bkthomps/Containers/tree/master.svg?style=shield)](https://circleci.com/gh/bkthomps/Containers/tree/master)
[![Documentation](https://codedocs.xyz/bkthomps/Containers.svg)](https://codedocs.xyz/bkthomps/Containers/)
[![Codecov](https://codecov.io/gh/bkthomps/Containers/branch/master/graph/badge.svg)](https://codecov.io/gh/bkthomps/Containers)
[![Codacy](https://api.codacy.com/project/badge/Grade/be77f904a65b4bd0b991df85e6cb37f0)](https://www.codacy.com/app/bkthomps/Containers?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=bkthomps/Containers&amp;utm_campaign=Badge_Grade)
[![Language](https://img.shields.io/badge/language-C89+-orange.svg)](https://en.wikipedia.org/wiki/C_(programming_language))
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](https://github.com/bkthomps/Containers/blob/master/LICENSE)

# Containers
This library provides various containers. Each container has utility functions to manipulate the data it holds. This is an abstraction as to not have to manually manage and reallocate memory. 

Inspired by the C++ standard library; however, implemented using C with different function interfaces as the C++ standard library but with the same container names.

## Setup
The `build.sh` script can be used to build either dynamic or static libraries. The script supports clang and gcc.

The benefit of a dynamic library is that changing the `containers.so` library can be done without needing to recompile the codebase which is using the library. Nevertheless, it is slower than a static library.

The benefit of a static library is that it is faster than a dynamic library. However, if the `containers.a` library is modified, the codebase which is using the library needs to be recompiled.

The installation process is as follows:
1. Clone this repository and navigate to it.
2. Run the `build.sh` build script.
3. Follow the instructions that the script prints.

## Container Types
The container types that this library contains are described below.

### Sequence containers
Data structures which can be accessed sequentially.
* array - static contiguous array
* vector - dynamic contiguous array
* deque - double-ended queue
* forward_list - singly-linked list
* list - doubly-linked list

### Associative containers
Data structures that can be quickly searched which use comparators.
* set - collection of unique keys, sorted by keys
* map - collection of key-value pairs, sorted by keys, keys are unique
* multiset - collection of keys, sorted by keys
* multimap - collection of key-value pairs, sorted by keys

### Unordered associative containers
Data structures that can be quickly searched which use hashing.
* unordered_set - collection of unique keys, hashed by keys
* unordered_map - collection of key-value pairs, hashed by keys, keys are unique
* unordered_multiset - collection of keys, hashed by keys
* unordered_multimap - collection of key-value pairs, hashed by keys

### Container adaptors
Data structures which adapt other containers to enhance functionality.
* stack - adapts a container to provide stack (last-in first-out)
* queue - adapts a container to provide queue (first-in first-out)
* priority_queue - adapts a container to provide priority queue

