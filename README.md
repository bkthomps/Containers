[![CircleCI](https://circleci.com/gh/bkthomps/Containers/tree/master.svg?style=svg)](https://circleci.com/gh/bkthomps/Containers/tree/master)

# Containers
This library provides various containers. Each container has utility functions to manipulate the data it holds. This is an abstraction as to not have to manually manage and reallocate memory. 

Inspired by the C++ standard library; however, implemented using C with different function interfaces as the C++ standard library but with the same container names.

## Sequence containers
Data structures which can be accessed sequentially.
* array - static contiguous array
* vector - dynamic contiguous array
* deque - double-ended queue
* forward_list - singly-linked list
* list - doubly-linked list

## Associative containers
Data structures that can be quickly searched which use comparators.
* set - collection of unique keys, sorted by keys
* map - collection of key-value pairs, sorted by keys, keys are unique
* multiset - collection of keys, sorted by keys
* multimap - collection of key-value pairs, sorted by keys

## Unordered associative containers
Data structures that can be quickly searched which use hashing.
* unordered_set - collection of unique keys, hashed by keys
* unordered_map - collection of key-value pairs, hashed by keys, keys are unique
* unordered_multiset - collection of keys, hashed by keys
* unordered_multimap - collection of key-value pairs, hashed by keys

## Container adaptors
Data structures which adapt other containers to enhance functionality.
* stack - adapts a container to provide stack (last-in first-out)
* queue - adapts a container to provide queue (first-in first-out)
* priority_queue - adapts a container to provide priority queue
