CE Algorithms Package
=====================
This library aims to provide reusable and efficient data-structures and algorithms for C programmers on the TI-84+CE platform. To make life easier, these libraries also compile on PC platforms to make it easier to prototype your program on a platform with more advanced debugging tools.

This library is licensed under the terms of the [LGPL v3](http://www.gnu.org/licenses/lgpl-3.0.txt) (or newer), with an added no-endorsement clause.

What's inside?
--------------

Currently provides implementations of [singly-linked lists](https://en.wikipedia.org/wiki/Linked_list#Singly_linked_list) and [red-black trees](https://en.wikipedia.org/wiki/Red%E2%80%93black_tree). A generic merge-sort implementation is provided for the linked-lists, and helper functions to use red-black trees as the backing structure for both ordered sets and ordered maps are provided.

Additionally, a SafeMalloc implementation is provided, in `util.h` which will display an error message and terminate that program if insufficient memory is available for a requested allocation. This is great for prototyping programs.
We also package the standard `cleanUp()` routine from the [CE Toolchain Examples](https://github.com/CE-Programming/toolchain/).

Finally, `debugConfig.h` provides a `dPrintf` macro that disables debug printing in release builds, and cooperates with [CEmu's debug console](https://github.com/MateoConLechuga/CEmu) on 84+CE debug builds, as well as `SZF` and `PDF` string macros for portable formatting of `size_t` and `ptrdiff_t` variables.

See the header files for more documentation.
