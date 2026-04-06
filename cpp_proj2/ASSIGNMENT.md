# Assignment: N-Way Set-Associative Cache Simulator

## Overview
Extend the direct-mapped cache simulator from Project 1 into a fully configurable N-way set-associative cache. The simulator reads a trace file of memory addresses and reports hit/miss statistics. Associativity is hardcoded for now but the structure must support N ways per set.

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
- Associativity: 4 ways

**Output:**
```
Total accesses: 4
Hits: 1
Misses: 3
Hit rate: 25.00%
```

---

## Address Breakdown

With 4-way associativity, the cache has 8 sets (1024 bytes / 32 bytes per block / 4 ways = 8 sets).

A 32-bit address breaks into three fields:

```
| 24 tag bits | 3 index bits | 5 offset bits |
```

- **offset** (bits 4–0): which byte within the 32-byte block
- **index** (bits 7–5): which of the 8 sets to look in
- **tag** (bits 31–8): stored alongside each block to verify identity

> Note: Compare with Project 1 — going from 1-way to 4-way reduced the number of sets from 32 to 8, which reduced index bits from 5 to 3, and widened the tag from 22 to 24 bits.

---

## Cache Structure

The cache is a 2D structure: **sets × ways**.

```
         Way 0        Way 1        Way 2        Way 3
Set 0  [tag|valid]  [tag|valid]  [tag|valid]  [tag|valid]
Set 1  [tag|valid]  [tag|valid]  [tag|valid]  [tag|valid]
...
Set 7  [tag|valid]  [tag|valid]  [tag|valid]  [tag|valid]
```

Each cell holds one cache block. On access, all ways in the target set are checked in parallel (simulated sequentially in code).

---

## Replacement Policy: LRU

When all ways in a set are valid and a miss occurs, evict the **least recently used** way.

Track LRU order per set using a `lru_order` array of size N per set. On every access (hit or miss), update the order so the most recently used way moves to the front.

---

## Steps

1. **Set up your project** — create `main.cpp` and folder structure, confirm hello world compiles ✅
2. **Update the cache block struct** — keep `uint32_t tag` and `bool valid`; add an `int lru_counter` field for LRU tracking
3. **Initialize the cache** — represent it as a 2D array `cache_block cache[NUM_SETS][NUM_WAYS]`, all valid = false, lru_counter = 0
4. **Recompute address decomposition** — extract index (3 bits) and tag (24 bits) based on new set count
5. **Implement set lookup** — for a given set, scan all ways: if any way has valid=true and matching tag → hit; otherwise → miss
6. **Implement LRU eviction** — on a miss, find the way with the lowest lru_counter (least recently used) and replace it; on every access update counters
7. **Read the trace file** — same as Project 1: open file, read hex addresses line by line, run each through lookup
8. **Print final statistics** — total accesses, hits, misses, hit rate
9. **Verify conflict miss reduction** — use an input trace that caused conflict misses in Project 1 and confirm the hit rate improves with 4-way associativity

---

## Key Concepts to Internalize
- Why does increasing associativity reduce index bits and increase tag bits?
- What is the cost of full associativity, and why does set-associative cache find the middle ground?
- Why does LRU outperform FIFO on typical workloads?
- What kind of miss does N-way associativity eliminate — compulsory, conflict, or capacity?
