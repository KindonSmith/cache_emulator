# Assignment: Direct-Mapped Cache Simulator

## Overview
Build a command-line C++ program that simulates a direct-mapped cache. It reads a trace file of memory addresses, processes each access, and reports hit/miss statistics.

---

## Spec

**Input:** A plain text file where each line is a hexadecimal memory address, e.g.:
```
0x0A3F
0x0A40
0x0B21
0x0A3F
```

**Parameters (hardcode for now):**
- Cache size: 1024 bytes
- Block size: 32 bytes
- Address size: 32-bit

**Output:**
```
Total accesses: 4
Hits: 1
Misses: 3
Hit rate: 25.00%
```

---

## Address Breakdown

For this cache, a 32-bit address breaks into three fields:

```
| 22 tag bits | 5 index bits | 5 offset bits |
```

- **offset** (bits 4–0): which byte within the 32-byte block
- **index** (bits 9–5): which of the 32 cache slots to look in
- **tag** (bits 31–10): stored in the block to verify it holds the right data

---

## Steps

1. **Set up your project** — create `main.cpp`, confirm you can compile and run a hello world with `g++` ✅
2. **Define a cache block struct** — fields: `uint32_t tag`, `bool valid`
3. **Initialize the cache** — represent it as an array of 32 blocks, all valid = false
4. **Parse the address** — given a 32-bit address, extract the index and tag using bit shift and mask operations
5. **Implement the lookup** — on each access, check the slot at [index]: if valid and tag matches → hit, otherwise → miss, then update the slot
6. **Read the trace file** — open a file, read addresses line by line, run each through your lookup
7. **Print the final statistics** — total accesses, hits, misses, hit rate
