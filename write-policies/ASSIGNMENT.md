# Assignment: Write Policies + Read/Write Trace Format

## Overview
Extend your configurable cache simulator to support **write policies**. The trace file now distinguishes between reads and writes. Track dirty blocks, count writebacks, and compare how different policy combinations affect traffic to the next level of the memory hierarchy.

---

## C++ Learning Goals

### 1. Input Handling — Parsing a Structured Trace Format
The trace file format changes from a bare address to a **read/write trace**:
```
R 0x0A3F
W 0x0A40
R 0x0B21
W 0x0A3F
```

Each line has two tokens: an operation (`R` or `W`) and a hex address.

**Parsing challenge:**
- Read two tokens per line — how do you extract a `char` and a `uint32_t` from the same line?
- Options: `fscanf`, `std::ifstream` with `>>` operator, `std::getline` + `std::istringstream`
- What happens if a line is malformed (e.g., missing address, unknown operation letter)? Handle it gracefully — skip the line, print a warning, keep going.

**Key C++ concepts:**
- `std::istringstream` — construct a stream from a string, then use `>>` to extract tokens
- `>>` operator on streams — how does it handle whitespace? What does the return value tell you?
- Character comparison: `op == 'R'` vs `op == "R"` — one compiles, one doesn't. Why?

### 2. Pass by Reference — Mutable State
This project deepens your understanding of when and why to pass by reference:

- `Cache::lookUp()` now needs to communicate **more** back to the caller: was it a hit? Was there a writeback? You have options:
  - Return a struct with multiple fields
  - Take output parameters by reference (`bool &writeback_occurred`)
  - Use a result enum + side effects

**Exercise:** Implement `Cache::lookUp()` so that it returns a struct (e.g., `lookup_result`) containing the hit/miss outcome and whether a writeback happened. Compare this to the alternative of using reference output parameters — which is clearer? Add a comment explaining your choice.

**Carry forward from Project 3:** Any function that receives a large container it does not modify should take it as `const std::vector<...> &` — no copy, no mutation. Apply this consistently to trace parsing and stats functions.

**Deeper question:** When you return a struct from a function, does C++ copy it? (Look up: "copy elision" and "named return value optimization" — but don't worry about memorizing the rules, just know the compiler is smarter than you think.)

### 3. Multi-File Organization — Adding a Module
You now add a new module to your Project 3 structure:

```
cpp_proj4/
├── src/
│   ├── main.cpp              # entry point, CLI parsing, orchestration
│   ├── structs/
│   │   ├── cache_block.h     # cache_block struct (add dirty bit)
│   │   ├── cache_set.h       # cache_set struct with updateLRU methods
│   │   └── cache.h           # Cache struct — config fields + all methods inline
│   ├── functions/
│   │   ├── stats.h / stats.cpp   # stats struct (add writeback counter), printStats
│   │   └── trace.h / trace.cpp   # NEW: trace parsing (readNextAccess)
│   └── input/
│       ├── trace_rw1.txt     # basic read/write mix
│       ├── trace_rw2.txt     # write-heavy workload
│       └── trace_rw3.txt     # read-heavy, high locality
└── ASSIGNMENT.md
```

**Key decisions:**
- Struct methods (`Cache::lookUp`, `cache_set::updateLRU`) stay in their headers — they are implicitly inline and do not cause multiple-definition errors. This is the pattern established in Project 3.
- Free utility functions (`printStats`, `readNextAccess`) go in `functions/` with a `.h`/`.cpp` split. Why does this split matter for free functions but not for struct methods?
- Write policy config lives **inside the `Cache` struct** as fields (consistent with Project 3 — no separate `cache_config` struct needed). Add `WriteHitPolicy` and `WriteMissPolicy` enum fields directly to `Cache`.
- `trace.h` declares `bool readNextAccess(std::ifstream &file, char &op, uint32_t &addr)` — notice the reference parameters. Why references here instead of return values?
- When `stats.cpp` includes `structs/cache.h`, and `main.cpp` also includes it — the include guard prevents double definition. Trace through what would happen without the guard.

### 4. Test Suite — Policy Comparison Matrix
Design traces and run a comparison matrix:

```
./cache_sim 1024 32 4 wb wa input/trace_rw1.txt
./cache_sim 1024 32 4 wb nwa input/trace_rw1.txt
./cache_sim 1024 32 4 wt wa input/trace_rw1.txt
./cache_sim 1024 32 4 wt nwa input/trace_rw1.txt
```

New CLI format:
```
./cache_sim <cache_size> <block_size> <assoc> <write_hit> <write_miss> <trace_file>
```
- `<write_hit>` : `wb` (write-back) or `wt` (write-through)
- `<write_miss>` : `wa` (write-allocate) or `nwa` (no-write-allocate)

**Invalid combo:** `wb` + `nwa` doesn't make sense — if you don't allocate on a write miss, you can never have a dirty block to write back. Detect and reject this combination.

Create a results table after testing:

| Policy Combo | Hits | Misses | Writebacks | Writes to Next Level |
|---|---|---|---|---|
| WB + WA | ? | ? | ? | ? |
| WT + WA | ? | ? | 0 | ? |
| WT + NWA | ? | ? | 0 | ? |

---

## Spec

**Input:** CLI arguments + R/W trace file.

**New CLI Arguments:**
```
./cache_sim <cache_size> <block_size> <assoc> <write_hit> <write_miss> <trace_file>
```

**Output:**
```
Cache Configuration:
  Cache size:      1024 bytes
  Block size:      32 bytes
  Associativity:   4 ways
  Number of sets:  8
  Write hit:       write-back
  Write miss:      write-allocate

Results:
  Total accesses:       100
  Reads:                60
  Writes:               40
  Hits:                 55
  Misses:               45
  Hit rate:             55.00%
  Writebacks:           12
  Writes to next level: 12
```

- **Writebacks**: number of dirty blocks evicted (write-back only; always 0 for write-through)
- **Writes to next level**: total writes sent downward — for write-through, every write goes down; for write-back, only writebacks go down

---

## Write Policy Details

### Write-Back (WB)
On a **write hit**: mark the block dirty, do **not** write to the next level.
On **eviction** of a dirty block: write it to the next level (count as a writeback).

### Write-Through (WT)
On a **write hit**: write to both the cache and the next level immediately.
No dirty bits needed — blocks are never stale.

### Write-Allocate (WA)
On a **write miss**: bring the block into the cache (just like a read miss), then perform the write in the cache.

### No-Write-Allocate (NWA)
On a **write miss**: write directly to the next level, do **not** bring the block into the cache.

---

## Data Structure Changes

### `cache_block` — add `dirty` bit
```
struct cache_block {
    uint32_t tag;
    bool valid;
    bool dirty;       // NEW
    int lru_counter;
};
```

### `Cache` struct — add write policy fields
Add two fields directly to the `Cache` struct (consistent with Project 3 — config lives in the struct, not a separate object).

**C++ concept — enums:**
```cpp
enum class WriteHitPolicy  { WRITE_BACK, WRITE_THROUGH };
enum class WriteMissPolicy { WRITE_ALLOCATE, NO_WRITE_ALLOCATE };
```
- `enum class` vs bare `enum` — why prefer `enum class`? (Hint: scope and type safety)
- Store these as fields on `Cache` and use them inside `Cache::lookUp()` to branch behavior
- Add them to the constructor and member initializer list — follow the same pattern used for `offsetBits`, `indexBits`, etc. in Project 3

### `lookup_result` — new return struct
```
struct lookup_result {
    bool hit;
    bool writeback_occurred;
};
```

### `stats` — add counters
```
struct stats {
    int total_accesses;
    int reads;
    int writes;
    int hits;
    int misses;
    int writebacks;
    int writes_to_next_level;
};
```

---

## Steps

1. **Copy and restructure from Project 3** — add `functions/trace.h`/`trace.cpp`, add write policy fields to `Cache` struct, verify compilation
2. **Add dirty bit to `cache_block`** — initialize to `false`
3. **Add enums for write policies** — define `WriteHitPolicy` and `WriteMissPolicy` in `config.h`
4. **Parse new CLI arguments** — validate policy strings, reject `wb + nwa` combo
5. **Implement trace parser** — `readNextAccess()` in `trace.cpp`, handles `R`/`W` + hex address per line, warns on malformed lines
6. **Update `cacheLookup`** — branch on read vs write, apply correct policy, set dirty bits, detect writebacks on eviction
7. **Update stats tracking** — separate read/write counters, writeback counter, writes-to-next-level counter
8. **Update `printStats`** — display new counters and policy names
9. **Create 3 R/W test traces** — one balanced, one write-heavy, one read-heavy
10. **Run the comparison matrix** — all valid policy combos × all traces, fill in results table

---

## Key Concepts to Internalize
- Why does write-back reduce traffic to the next level compared to write-through?
- Why is write-back + no-write-allocate an invalid combination?
- In a write-through cache, why are writebacks always 0?
- When does write-allocate help? When does it hurt? (Think: temporal locality of writes)
- What's the difference between `enum` and `enum class` in C++? Why does `enum class` prevent bugs?
- When a function needs to communicate multiple pieces of information back to the caller, what are the trade-offs between: returning a struct, using reference output parameters, and using a global variable?
