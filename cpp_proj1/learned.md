### What I learned on this assignment:

#### Caches:
Caches - I learned how cache blocks are used to determine the number of bits pulled from an address.
32 slots = 5 bits, 64 slots = 6 bits, and so on.

Specifications determine which bits to pull from addresses for indexing and tags.

We have to use uint32_t in cpp to guarantee sizing, otherwise we might end up with weird/out of range results in our cache indexing.

To pull index, bitshift address and mask.
To pull tags, bitshift to tag bits.

Lookup just uses the index for o(1) memory access, first we check validity boolean (for performance reasons), then we check data (more expensive comparison I think?). If valid (i.e. this has been populated at all before) and data matches, we hit and can return 1 immediately!

if not valid or no match, we set the cache[index].tag to this data and set cache[index].valid to true, then return 0. we missed but cached.

#### Bits
learned more about bitshifting and how to shift in cpp

learned masking is 0 for any bit you don't want and 1 for any bit you want. bits & mask. The logic means that for masked bits, 0 becomes 0 and 1 becomes 0. For unmasked bits, 0 stays 0 and 1 stays 1.
this computes to keeping only the bits you want.

#### CPP
review into header files and function declarations

review into passing by reference vs value

learned that arrays default to passing by reference

learned standard cpp format of passing array by ref in function stack.

learned bitshifting in cpp

including header file and file pathing

input reading from file.

#### VS code
changed keybind for text sizing

learned to use g++ to compile and run.