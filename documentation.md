# Documentation
For setup and compilation instructions visit the [readme](README.md). For
in-depth documentation, visit the
[code docs](https://codedocs.xyz/bkthomps/Containers/) page.

## General Overview
Each container has an initialization function which returns a container object.
For a deque, this would be `deque_init()` which returns a `deque`. The returned
object is a pointer to an internal struct which contains information, but this
is abstracted away to reduce mistakes and make development easier. More
information about the initialization functions is presented below in its own
section.

Once a container object is initialized, it is possible to manipulate it using
the provided functions, which have in-depth documentation available. Each
container has adding and retrieval type functions, and each type of container
has its own specific set of function interfaces, which are explained in-depth at
the function level in the code docs link above. More high-level information will
be explained in its own section below.

Finally, each container will have to be destroyed to free the memory associated
with it.

# Container Initialization
When creating a container, first you must decide what type of data you wish to
store in it (or types for the case of a map). Then, you must either decide if
you wish to store a copy of the data in the container, or a pointer to it. The
benefit of a copy is that you don't have to manage the memory, and it is easier
to reason. However, it might only be reasonable to store copies if the data type
is relatively cheap to copy. Using pointers, you can either store references to
automatic variables (make sure the lifetime stays valid while the data is
stored), or dynamically-allocated variables (make sure to free at some point).
Either way, you must pass in a pointer of what you wish to store, be it a
pointer to a value, or a pointer to a pointer. Keep in mind that for some
containers, changing data which is stored will cause undefined behavior. Fret
not, as all containers document their potential undefined behavior in the
function documentation comments.

Going back to the initialization function and ways we can initialize containers
and and store data, we can use an example of a `deque`:
```
deque a = deque_init(sizeof(int)); /* OK: cheap to copy */
deque b = deque_init(sizeof(struct expensive_data)); /* Bad: valid, but expensive to copy */
deque c = deque_init(sizeof(struct expensive_data *)); /* OK: pointer to expensive data, but be careful about memory management */
```

Also, if the arguments which you passed in to the initialization function are
invalid, or the system is out of memory, the initialization function may return
NULL. Therefore, it is good to check for a return of NULL from the
initialization functions.

# Container Operations
Next, the two basic container manipulation functions of containers are to add
and remove elements. To add elements, pass a pointer of the element you wish to
copy to the add function. To remove an element, pass a pointer to a variable
which can store the size of element that is stored in the container.

Using the `deque` example, adding and removal can be done this way (if storing
int):
```
deque d = deque_init(sizeof(int));
...
int add = 5;
bk_err rc = deque_push_back(d, &add); /* 5 has been added to the back of the deque */
...
int retrieve;
bk_err rc = deque_pop_back(&retrieve, d); /* retrieve now is equal to 5 */
...
```

Functions can fail for various reasons, such as the provided index argument
being out of bounds, or the system running out of memory. The in-depth
documentation linked above provides the exhaustive list of return codes for each
function, which are present in the header file. For example, an invalid argument
would return `-BK_EINVAL`, and on success `BK_OK` would be returned.

# Comparators and Hash Functions
The associative containers and the priority queue require the user to initialize
the container with a comparator, and the unordered associative containers also
require a hash function to be passed in. State should not be modified in
comparators or in hash functions, or else it would lead to undefined behavior.

When a comparator function is called, two arguments are passed in, being two
elements to compare. The comparator must return 0 if they are equal, a negative
value if the first is less than the second, and a positive value is the first is
greater than the second. To be valid, a comparator must obey the following
rules:
1. Reflexive: `arg_1 == arg_1` and `arg_2 == arg_2` must always be true
2. Symmetric: if `arg_1 == arg_2` is true, then `arg_2 == arg_1` is true
3. Transitive: if the comparator is called twice with three distinct objects,
the first time being `(arg_1, arg_2)` and the second `(arg_2, arg_3)`, then if
`arg_1 == arg_2` and `arg_2 == arg_3` then `arg_1 == arg_3` must be true
4. Consistent: the truth of `arg_1 == arg_2` shall never change over time if
both arguments do not change

When a hash function is called, it is provided one argument, and must hash its
attributes. A hash is a mapping from the object to a number. Two objects which
are equal must always result in the same hash being produced. The inverse is not
required, but should be sufficiently improbable (meaning, two distinct objects
may produce the same hash, but it should be very rare).

Using unordered set as an example (and storing int), the comparator and hash
function can be passed in as follows when initializing:
```
unordered_set_init(sizeof(int), hash_int, compare_int)
```

A comparator can be as follows:
```
static int compare_int(const void *const one, const void *const two)
{
    const int a = *(int *) one;
    const int b = *(int *) two;
    return a - b;
}
```

And a hash function can be as follows:
```
static unsigned long hash_int(const void *const key)
{
    unsigned long hash = 17;
    hash = 31 * hash + *(int *) key;
    return hash;
}
```
