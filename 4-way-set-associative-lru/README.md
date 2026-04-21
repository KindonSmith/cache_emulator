# 4-way-set-associative-lru
Building on direct-mapping, this cache supports 4-way associativity (8 sets of 4 ways) for a fixed-sized cache.

## Build
```
bash cd src
g++ main.cpp -o 4w-cache.exe
```

## Usage
```
./4w-cache.exe
```

## Spec 
**Cache configuration:**
```
- Cache size: 1024 bytes
- Block size: 32 bytes
- Address size: 32-bit
- Associativity: 4-way
```

**Address Breakdown:**

For this cache, a 32-bit address breaks into three fields:

```
| 24 tag bits | 3 index bits | 5 offset bits |
```

- **offset** (bits 4–0): which byte within the 32-byte block
- **index** (bits 7–5): which of the 32 cache slots to look in
- **tag** (bits 31–8): stored in the block to verify it holds the right data


**Input:** A plain text file where each line is a hexadecimal memory address, e.g.:
```
0x0A3F
0x0A40
0x0B21
0x0A3F
```
Sample [input file](./src/input/input.txt)

**Output:**
```
Total accesses: 32
Hits: 29
Misses: 3
Hit rate: 90.625%
```

## Design
#### Data Structures
##### New
- cache_set.h: a struct containing 4 ways, updating LRU trackers for it's ways when accessed.

##### Update:
- cache_block.h: now tracks last usage via an integer counter. Managed by parental set.


#### LRU
- Track the Least Recently Used (LRU) way in each set and if we need to overwrite, we log and reset this value. 
- Increment each way's LRU counter every time its set is used. LRU only matters locally within a set. We don't increment the LRU on every cache read, only local (set-level) accesses.

#### Address decomposition
##### Updates:
- Index extraction. The address (uint_32t) is bit-shifted right 5 and masked (0x7) to provide 3 index bits.
- Tag extraction. The address is bit-shifted right 8 to provide 24 tag bits.

#### Output
- Final stats of accesses, hits, and misses.

#### Scope
- No write policies - this is a read-only trace
- Set associativity for fixed-size cache.
- LRU policy implementation on set
