# Assignment: Configurable Cache Simulator (Multi-File)

## Overview
Refactor and extend your Project 2 simulator into a **configurable, multi-file** cache simulator. Cache parameters come from the command line instead of being hardcoded. Code is split across multiple files with proper headers.

---

## C++ Learning Goals

### 1. Input Handling — Command-Line Arguments
Parse `argc` and `argv` in `main()` to accept cache configuration at runtime:
```
./cache_sim 1024 32 4 input/trace1.txt
```
Arguments: cache size (bytes), block size (bytes), associativity, trace file path.

Validate inputs:
- Cache size and block size must be powers of 2
- Associativity must divide evenly into (cache size / block size)
- File must exist

Print a usage message if arguments are missing or invalid:
```
Usage: ./cache_sim <cache_size> <block_size> <associativity> <trace_file>
```

**Key C++ concepts:**
- `argc` / `argv` — what type is `argv`? How do you convert `argv[1]` to an integer?
- `std::stoi()` and `std::stoul()` for string-to-number conversion
- `atoi()` vs `std::stoi()` — why prefer the latter?

### 2. Pass by Value vs. Reference
This project will make you confront how C++ passes data to functions.

**Rules to internalize:**
- Primitive types (`int`, `uint32_t`, `bool`) → passed by **value** (copy)
- Structs and classes → passed by **value** (copy) unless you use `&`
- Arrays → decay to a **pointer** when passed to a function (the pointer is a copy, but it points to the original data — so modifications to elements affect the caller)
- `const &` → pass by reference without allowing modification (efficient + safe)

**Where this matters in your code:**
- Your `cache_config` struct should be passed by `const &` to functions that read it — why not by value?
- Your `cache_set` array is modified by `cacheLookup` — should it be passed by reference or pointer? What happens if you accidentally pass a single `cache_set` by value?
- Your stats counters need to be updated by the lookup function — how do you ensure changes persist?

**Exercise:** At least one function must take a parameter by value and one by `const &`. Add a comment above each function signature explaining *why* you chose value, reference, or const reference for each parameter.

### 3. Multi-File Project Structure
Split your code across files like this:

```
cpp_proj3/
├── src/
│   ├── main.cpp              # entry point, argument parsing, orchestration
│   ├── cache.h               # cache_block, cache_set structs + function declarations
│   ├── cache.cpp             # cacheLookup, extractIndex, extractTag implementations
│   ├── config.h              # cache_config struct definition
│   ├── stats.h               # stats struct definition
│   ├── stats.cpp             # printStats implementation
│   └── input/
│       ├── trace1.txt        # basic test trace
│       ├── trace2.txt        # conflict-heavy trace
│       └── trace3.txt        # sequential scan trace
└── ASSIGNMENT.md
```

**Key C++ concepts:**
- **Header files (`.h`):** Contain struct definitions, function **declarations** (prototypes), and `#include` guards
- **Source files (`.cpp`):** Contain function **definitions** (implementations), and `#include` their own header
- **Include guards:** `#ifndef CACHE_H` / `#define CACHE_H` / `#endif` — prevents double-inclusion
- **One header, multiple functions:** Yes — a single header can declare several related functions. Group by responsibility (all cache functions in `cache.h`, all stats functions in `stats.h`)
- **Compiling multiple files:** `g++ main.cpp cache.cpp stats.cpp -o cache_sim` — the linker resolves cross-file function calls

**Questions to answer before coding:**
- What goes in a `.h` file vs a `.cpp` file? Can you put a function *definition* in a header? What happens if you do and include it in two `.cpp` files?
- If `cache.cpp` uses `cache_config`, does it `#include "config.h"` or `#include "cache.h"`?
- Why do include guards matter? What error do you get without them?

### 4. Test Suite with Multiple Traces
Create at least 3 input trace files that test different behaviors:

| Trace File | Purpose | What It Tests |
|---|---|---|
| `trace1.txt` | Basic functionality | Mix of hits and misses, verify correctness |
| `trace2.txt` | Conflict-heavy | Many addresses mapping to the same set — stress LRU eviction |
| `trace3.txt` | Sequential scan | Addresses that march through memory linearly — when does LRU fail? |

Run the simulator multiple times with different configurations on the same trace:
```
./cache_sim 1024 32 1 input/trace2.txt    # direct-mapped
./cache_sim 1024 32 4 input/trace2.txt    # 4-way
./cache_sim 1024 32 8 input/trace2.txt    # 8-way
./cache_sim 1024 32 32 input/trace2.txt   # fully associative
```

**Expected insight:** The conflict-heavy trace should show improving hit rates as associativity increases. The sequential scan trace should show diminishing returns — why?

---

## Spec

**Input:** Command-line arguments for configuration + a trace file of hex addresses (same format as before).

**Computed Parameters (derive from arguments):**
- Number of sets = cache size / block size / associativity
- Offset bits = log2(block size)
- Index bits = log2(number of sets)
- Tag bits = 32 - offset bits - index bits

**Output:**
```
Cache Configuration:
  Cache size:     1024 bytes
  Block size:     32 bytes
  Associativity:  4 ways
  Number of sets: 8
  Offset bits:    5
  Index bits:     3
  Tag bits:       24

Results:
  Total accesses: 20
  Hits:           12
  Misses:         8
  Hit rate:       60.00%
```

---

## Data Structures

### `cache_config` (defined in `config.h`)
```
struct cache_config {
    int cache_size;
    int block_size;
    int associativity;
    int num_sets;
    int offset_bits;
    int index_bits;
    int tag_bits;
};
```
Populate the derived fields after parsing arguments. Pass this struct to functions that need configuration — by `const &`.

### `cache_block` and `cache_set` (defined in `cache.h`)
Same as Project 2, but `cache_set` should hold a **dynamically sized** array of ways (since associativity isn't hardcoded).

**C++ decision:** How do you make a struct hold N items when N isn't known at compile time? Options:
- `std::vector<cache_block>` — standard, safe, heap-allocated
- `new cache_block[n]` — manual allocation, must `delete[]`
- Fixed-size array with a max — simple but wasteful

Pick one. Be ready to explain your choice.

### `stats` (defined in `stats.h`)
```
struct stats {
    int total_accesses;
    int hits;
    int misses;
};
```

---

## Steps

1. **Set up the multi-file structure** — create all files, add include guards, verify it compiles with `g++ main.cpp cache.cpp stats.cpp -o cache_sim`
2. **Define structs** — `cache_config` in `config.h`, `cache_block`/`cache_set` in `cache.h`, `stats` in `stats.h`
3. **Parse command-line arguments** — validate and populate a `cache_config` struct in `main.cpp`
4. **Compute derived parameters** — num_sets, offset/index/tag bits from the config
5. **Initialize the cache** — dynamically allocate an array of `cache_set`, each with the correct number of ways
6. **Port cache logic** — move `extractIndex`, `extractTag`, `cacheLookup` to `cache.cpp`, declare in `cache.h`. Generalize to use `cache_config` instead of hardcoded constants
7. **Port stats** — move `printStats` to `stats.cpp`, declare in `stats.h`
8. **Read trace and simulate** — same as before, but using the parsed file path and config
9. **Create 3 test traces** — build `trace1.txt`, `trace2.txt`, `trace3.txt` with distinct purposes
10. **Run comparative tests** — vary associativity on the same trace, observe hit rate changes

---

## Key Concepts to Internalize
- What is the difference between a **declaration** and a **definition** in C++?
- Why do structs get copied when passed by value? What does "the stack frame gets its own copy" mean?
- If you pass a `cache_set` by value to `cacheLookup` and modify it, what happens to the original?
- How does the compiler know what functions exist in another `.cpp` file? (Hint: it doesn't — the *linker* does. The compiler only needs the declaration from the header.)
- Why does increasing associativity help for conflict-heavy traces but not sequential scans?
- What is the fully associative case? How many index bits does it have?
