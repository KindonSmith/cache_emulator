# Assignment: Two-Level Cache Hierarchy + AMAT

## Overview
Build a **two-level cache simulator** (L1 + L2). On an L1 miss, the access falls through to L2. On an L2 miss, it goes to main memory. Compute the **Average Memory Access Time (AMAT)** using hit rates and assumed latencies. This project ties together everything from Projects 1–4 and introduces object composition and more advanced C++ patterns.

---

## C++ Learning Goals

### 1. Input Handling — Configuration File
The number of CLI arguments is getting unwieldy. Introduce a **configuration file** as an alternative input method:

```
# cache_config.txt
L1_SIZE=1024
L1_BLOCK_SIZE=32
L1_ASSOC=4
L1_WRITE_HIT=wb
L1_WRITE_MISS=wa
L2_SIZE=8192
L2_BLOCK_SIZE=64
L2_ASSOC=8
L2_WRITE_HIT=wb
L2_WRITE_MISS=wa
L1_HIT_TIME=1
L2_HIT_TIME=10
MEMORY_ACCESS_TIME=100
```

**CLI invocation:**
```
./cache_sim --config configs/default.cfg input/trace1.txt
```

Or keep direct CLI args as a fallback:
```
./cache_sim --l1 1024 32 4 wb wa --l2 8192 64 8 wb wa --timing 1 10 100 input/trace1.txt
```

**Parsing challenge:**
- Read key=value pairs from a text file. Skip lines starting with `#` (comments) and blank lines.
- Use `std::getline` + `std::string::find('=')` + `std::string::substr()` to split each line.
- Convert values with `std::stoi`. Handle unknown keys gracefully (warn and skip).

**Key C++ concepts:**
- `std::string` methods: `find()`, `substr()`, `c_str()`
- File I/O: `std::ifstream`, checking `is_open()`, reading line by line
- `std::map<std::string, std::string>` — optionally load all key-value pairs into a map, then extract what you need. This previews associative containers from the STL.

### 2. Pass by Reference — Object Composition
The big design question: how do L1 and L2 relate to each other?

**Option A — L2 pointer inside L1's context:**
```cpp
struct cache_level {
    cache_config config;
    cache_set* sets;       // dynamically allocated
    stats level_stats;
};
```
Then your simulation function takes both levels:
```cpp
void simulateAccess(cache_level &L1, cache_level &L2, char op, uint32_t addr);
```
On L1 miss → call the same lookup logic on L2. On L2 miss → increment memory access count.

**Design questions:**
- Why pass `cache_level` by reference and not by value? What would happen to your stats if you passed by value?
- Could you pass by pointer instead? What's the syntactic difference (`level.stats.hits` vs `level->stats.hits`)? When would you prefer one over the other?
- If L1's block size differs from L2's, does an L1 miss always bring in exactly one L2 block? (Think about this — it connects to sub-blocking and sectored caches, but for this project assume L2 block size ≥ L1 block size.)

### 3. Multi-File Organization — Scaling Up
The project structure grows to support two cache levels and config parsing:

```
cpp_proj5/
├── src/
│   ├── main.cpp              # entry point, dispatch to config parser or CLI parser
│   ├── cache.h / cache.cpp   # cache_level struct, lookup logic, index/tag extraction
│   ├── config.h              # cache_config struct (per-level)
│   ├── config_parser.h       # NEW: parse config file into cache_config structs
│   ├── config_parser.cpp     # NEW: implementation
│   ├── stats.h / stats.cpp   # per-level stats + combined AMAT reporting
│   ├── trace.h / trace.cpp   # trace file parsing (reuse from proj4)
│   ├── input/
│   │   ├── trace1.txt        # moderate-size mixed R/W trace
│   │   ├── trace2.txt        # pathological for L1 (thrashing)
│   │   └── trace3.txt        # working set fits in L1 (L2 irrelevant)
│   └── configs/
│       ├── default.cfg       # baseline: L1=1KB/4-way, L2=8KB/8-way
│       ├── big_l2.cfg        # same L1, L2=32KB/16-way
│       └── tiny_l1.cfg       # L1=256B/2-way, L2=8KB/8-way
└── ASSIGNMENT.md
```

**Key patterns:**
- **Reusing code across levels:** The same `cacheLookup` function works for both L1 and L2 — it just takes a different `cache_level` by reference. This is the power of parameterization over duplication.
- **Header dependencies:** `cache.cpp` includes `cache.h` which includes `config.h`. `config_parser.cpp` includes `config_parser.h` which includes `config.h`. Both paths lead to `config.h` — include guards prevent breakage.
- **Compilation:** `g++ main.cpp cache.cpp stats.cpp trace.cpp config_parser.cpp -o cache_sim`

### 4. Test Suite — AMAT Comparison
Design tests that reveal when L2 helps and when it doesn't:

**Test configurations:**

| Config | L1 | L2 | Timing |
|---|---|---|---|
| `default.cfg` | 1KB, 32B block, 4-way | 8KB, 64B block, 8-way | 1 / 10 / 100 cycles |
| `big_l2.cfg` | 1KB, 32B block, 4-way | 32KB, 64B block, 16-way | 1 / 10 / 100 cycles |
| `tiny_l1.cfg` | 256B, 32B block, 2-way | 8KB, 64B block, 8-way | 1 / 10 / 100 cycles |

**Test traces:**

| Trace | Behavior | Expected Insight |
|---|---|---|
| `trace1.txt` | Mixed R/W, moderate locality | L2 catches most L1 misses |
| `trace2.txt` | L1-thrashing (working set > L1 but < L2) | L2 saves the day, AMAT stays low |
| `trace3.txt` | Working set fits in L1 | L2 barely accessed, AMAT ≈ L1 hit time |

Fill in a results table:

| Config | Trace | L1 Hit Rate | L2 Hit Rate | AMAT |
|---|---|---|---|---|
| default | trace1 | ? | ? | ? |
| default | trace2 | ? | ? | ? |
| ... | ... | ... | ... | ... |

---

## Spec

**Input:** Config file (or CLI args) + R/W trace file.

**Output:**
```
=== Cache Configuration ===
L1: 1024 bytes, 32B blocks, 4-way, write-back + write-allocate
    Sets: 8 | Offset bits: 5 | Index bits: 3 | Tag bits: 24
L2: 8192 bytes, 64B blocks, 8-way, write-back + write-allocate
    Sets: 16 | Offset bits: 6 | Index bits: 4 | Tag bits: 22
Timing: L1 hit = 1 cycle, L2 hit = 10 cycles, Memory = 100 cycles

=== Results ===
L1:
  Accesses: 1000
  Hits: 850     Misses: 150
  Hit rate: 85.00%
  Writebacks to L2: 30

L2:
  Accesses: 150
  Hits: 120     Misses: 30
  Hit rate: 80.00%
  Writebacks to memory: 8

AMAT: 1 + 0.15 * (10 + 0.20 * 100) = 4.50 cycles
```

---

## AMAT Formula

$$AMAT = HitTime_{L1} + MissRate_{L1} \times (HitTime_{L2} + MissRate_{L2} \times MemoryAccessTime)$$

Where:
- $MissRate_{L1}$ = L1 misses / L1 accesses
- $MissRate_{L2}$ = L2 misses / L2 accesses (this is the **local** miss rate)

**Show the formula with numbers in the output** so you can verify by hand.

**Question to think about:** Why do we use L2's *local* miss rate (L2 misses / L2 accesses) rather than *global* miss rate (L2 misses / total accesses)? Substitute into the formula and convince yourself they give the same AMAT.

---

## Multi-Level Cache Behavior

### Inclusive Policy (implement this)
L2 is a **superset** of L1:
- Every block in L1 is also in L2
- On L1 miss + L2 hit: block is loaded into L1 (already in L2, no L2 change needed)
- On L1 miss + L2 miss: block is loaded into both L1 and L2

### What This Means for Your Code
When L1 misses:
1. Compute L2 index and tag (L2 has different offset/index/tag widths!)
2. Look up in L2
3. If L2 hit → count L2 hit, load block into L1
4. If L2 miss → count L2 miss, load block into both L1 and L2

When an L1 eviction produces a writeback (dirty block):
- Write the dirty data to L2 (update the corresponding L2 block, mark it dirty)
- Count this as a writeback from L1

When an L2 eviction produces a writeback (dirty block):
- Write to main memory
- Count this as a writeback from L2

**Key insight:** L1 and L2 have *different* address decompositions because they have different block sizes, set counts, and associativities. The same address produces different index/tag pairs at each level.

---

## Data Structure Changes

### `cache_level` — one struct per level
```
struct cache_level {
    cache_config config;
    cache_set* sets;
    stats level_stats;
};
```

This is the composition pattern: a `cache_level` **has-a** `cache_config`, an array of `cache_set`s, and a `stats` struct. Functions operate on `cache_level &` — any level, any configuration.

### `cache_config` — add timing
```
int hit_time;          // cycles for a hit at this level
int memory_access_time; // only meaningful for the last level
```

---

## Steps

1. **Copy structure from Project 4** — bring over all files, add `config_parser.h/.cpp` and `configs/` folder
2. **Implement config file parser** — read key=value pairs, populate two `cache_config` structs (L1 and L2) and timing values
3. **Create `cache_level` struct** — bundle config, sets array, and stats into one unit
4. **Initialize two cache levels** — allocate sets/ways for L1 and L2 based on their respective configs
5. **Refactor `cacheLookup`** — takes a `cache_level &` parameter so the same function works for either level
6. **Implement two-level simulation** — on L1 miss, fall through to L2; on L2 miss, count memory access; handle writebacks cascading down
7. **Compute and display AMAT** — show the formula with substituted values
8. **Create config files** — `default.cfg`, `big_l2.cfg`, `tiny_l1.cfg`
9. **Create test traces** — 3 traces targeting different L1/L2 behaviors
10. **Run comparison matrix** — 3 configs × 3 traces, fill in results table, analyze trends

---

## Key Concepts to Internalize
- Why does AMAT use **local** miss rate for L2 (and how can you prove it's equivalent to using global miss rate)?
- Why does L2 typically have larger blocks than L1? (Think about spatial locality and the cost of a miss at each level.)
- Why is L2 typically more associative than L1? (What's the cost of high associativity, and why is L2 more tolerant of it?)
- In the inclusive policy, what happens if L2 evicts a block that's currently in L1? (You don't need to implement this — but think about why it's called "back-invalidation.")
- What does it mean to pass a `cache_level` struct by reference? What if you accidentally passed it by value to `cacheLookup` — would hits get counted?
- In your `cache_level` struct, `sets` is a pointer. If you copy a `cache_level` by value, do you get a deep copy or a shallow copy? What could go wrong?
