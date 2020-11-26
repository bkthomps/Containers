[![GitHubBuild](https://github.com/bkthomps/Containers/workflows/build/badge.svg)](https://github.com/bkthomps/Containers)
[![Documentation](https://codedocs.xyz/bkthomps/Containers.svg)](https://codedocs.xyz/bkthomps/Containers/)
[![Codecov](https://codecov.io/gh/bkthomps/Containers/branch/master/graph/badge.svg)](https://codecov.io/gh/bkthomps/Containers)
[![Language](https://img.shields.io/badge/language-C89+-orange.svg)](https://en.wikipedia.org/wiki/C_(programming_language))
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](https://github.com/bkthomps/Containers/blob/master/LICENSE)

# Containers
This library provides various containers. Each container has utility functions
to manipulate the data it holds. This is an abstraction as to not have to
manually manage and reallocate memory.

Inspired by the C++ standard library; however, implemented using C with
different function interfaces as the C++ standard library but with the same
container names.

## Setup
It is possible to compile this library as either static `.a` or dynamic `.so`:
1. A static library is slightly faster than a dynamic one, however, if the
library is modified, the entire project codebase which uses it will need to be
relinked.
2. A dynamic library can be changed without relinking the codebase, assuming
no function definitions have changed.

The installation process is as follows:
1. Clone this repository and navigate to it.
2. Run `make static_clang`/`make static_gcc` or
`make dynamic_clang`/`make dynamic_gcc` for either a static or dynamic library.
3. Then, you can copy-paste `containers.h` and `containers.a`/`containers.so`
into your project to include the containers.
4. Finally, you remember to link the library by including
`containers.a -ldl`/`containers.so -ldl` as an argument.

## Documentation
For high-level documentation and usage, visit the
[documentation](documentation.md) page. For in-depth documentation, visit the
[code docs](https://codedocs.xyz/bkthomps/Containers/) page.

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
