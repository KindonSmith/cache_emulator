# Direct-mapped Cache

A direct-mapped cache emulator where each address maps to a single slot.

## Build
```bash cd src
g++ main.cpp -o dm-cache.exe
```

## Usage
```
./dm-cache.exe
```

## Spec 
**Cache configuration:**
- Cache size: 1024 bytes
- Block size: 32 bytes
- Address size: 32-bit

**Address Breakdown:**

For this cache, a 32-bit address breaks into three fields:

```
| 22 tag bits | 5 index bits | 5 offset bits |
```

- **offset** (bits 4–0): which byte within the 32-byte block
- **index** (bits 9–5): which of the 32 cache slots to look in
- **tag** (bits 31–10): stored in the block to verify it holds the right data


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
Total accesses: 4
Hits: 1
Misses: 3
Hit rate: 25.00%
```

## Design
#### Data Structures
- cache_block.h. The cache_block is a stateless, so use of *struct* insted of *class* follows classic cpp design principles.
- cache_block [] (array) vs std::vector<cache_block> (vector). The cache is a hard-coded fixed size, so if emulating a performance-critical cache, it makes sense to optimize with an array over a vector. In terms of actual performance, the scope of this project trivializes the difference.
- uint_32t is CPP unsigned int with fixed 32bit size, guaranteeing our addresses are the same size for bitwise operations.

#### Input Parsing
- Filepath hardcoded to focus on implementing and understanding cache emulation.
- Hex addresses streamed with std::hex manipulator, allowing direct conversion to uint_32t.
- Assumes clean input - a cache is intended to be operating quickly and often. Validating each address would be both time-consuming and result in unexpected behavior. Any given application providing instructions should be assumed to provide correct input.

#### Address decomposition
- Index extraction. The address (uint_32t) is bit-shifted right 5 and masked (0x1F) to provide 5 bits per instruction. (1 << 5) - 1
- Tag extraction. The address is bit-shifted right 10 to provide 22 bits per instruction.

#### Output
- Final stats of accesses, hits, and misses.

#### Scope
- No write policies - this is a read-only trace
- No replacement policy. Direct-mapping is one-way, so there is only ever one way per instruction.