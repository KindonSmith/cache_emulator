# configurable-cache-set-associative-lru
Building on 4-way-set-associative-lru, this project supports
a configurable cache setup.

## Build
```bash 
cd src
g++ main.cpp -o emulate-cache.exe 
```

## Usage
```
./emulate-cache.exe [cache-size] [block-size] [associativity] [inputfile]

i.e. 

./emulate-cache.exe 1024 32 4 input/input.txt
```

## Spec 

**Address Breakdown:**

For this cache, a 32-bit address breaks into three fields:
1. Offset Bits
2. Index Bits
3. Tag Bits



Using the following example config:
- Cache Size: 1024
- Block Size: 32
- Associativity: 4

We can calculate our bits:

$$ offsetBits = \log_2(BlockSize) $$
$$ offsetBits = \log_2(32) $$
$$ offsetBits = 5 $$

$$ indexBits = \log_2(NumSets)$$
$$ NumSets = \frac{1024}{32 \times 4} $$
$$ NumSets = 8 $$
$$ indexBits = \log_2(8)$$
$$ indexBits = 3 $$

$$ tagBits = AddressSize - indexBits - offsetBits $$
$$ tagBits = 32 - 5 - 3 $$
$$ tagBits = 24 $$


```

| 24 tag bits | 3 index bits | 5 offset bits |


offset: directs to the byte offset in the block.
index: corresponds to the set within the cache.
tag: stored in the block to verify it holds the right data

```

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
##### New:
- 
##### Update:
- cache: cache now supports dynamic configuration.

#### Address decomposition
##### Update:
- Index, Offset, and Tag bit calculation. The bits (uint_32t) are dynamically calculated based on above formulas.

#### Output
- Final stats of accesses, hits, and misses.

#### Scope
- Configurable cache size with automatic backend calculation.
- Write policies not yet configurable, read-only trace.
