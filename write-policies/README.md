# write-policies
Building on a configurable cache with set associativity, this project adds support for
configurable write-hit and write-miss policies and moves input configurations to the input file.

Supported write-hit policies:
- Write-Back
- Write-Through

Supported write-miss policies:
- Write-Allocate
- No-Write-Allocate

## Build
```bash 
cd src
g++ main.cpp -o emulate-cache.exe 
```

## Usage
```
./emulate-cache.exe [inputFilePath]

i.e. 

./emulate-cache.exe trace_rw1.txt
```

## Spec 

**Write Policies:**
Write-hit policies determine behavior when a write operation hits on an existing cache line, and write-miss policies determine behavior when a write operation misses. The following policies are supported:

- Write-Hit Policies: Write-Back, Write-Through
- Write-Miss Policies: Write-Allocate, No-Write-Allocate

Below is a high-level summary of what these actually do:
Write-Hit:
- Write-Back: The cache line is marked as *dirty* and only written to main memory at eviction.
- Write-Through: Every write is immediately written to main memory.

Write-Miss:
- Write-Allocate: On a write miss, the cache allocates a new line and writes to it.
- No-Write-Allocate: On a write miss, the data is written directly to main memory, skipping the cache entirely.

> The Write-Back policy is inherently opposed to No-Write-Allocate, as NWA will never allow the cache to allocate a new line on a write miss. Cache lines that are not allocated cannot be marked as *dirty*, and WB will never write back. The WB + NWA combination is not supported in this cache. Only the following combinations are valid:
> - Write-Back with Write-Allocate (wb wa)
> - Write-Through with Write-Allocate (wt wa)
> - Write-Through with No-Write-Allocate (wt nwa)

**Address Breakdown:**

For this cache, a 32-bit address breaks into three fields:
1. Offset Bits
2. Index Bits
3. Tag Bits

These bits are calculated dynamically based on the cache configuration; see [configurable-cache](../configurable-cache-set-associative-lru/README.md##Spec) for details.

**Input:** A plain text file where the first line is the cache configuration and each subsequent line is a `R`/`W` operation followed by a hexadecimal memory address. Skeleton of the input file:
```text
[Cache Size] [Block Size] [Associativity] [Write-Hit Policy] [Write-Miss Policy]
R|W 0x<address>
R|W 0x<address>
R|W 0x<address>
```
*See a sample input file [here](./src/input/trace_rw1.txt).*

**Output:**
```
Cache Configuration:            
  Cache size:                  128 bytes
  Block size:                  32 bytes
  Associativity:               1 ways
  Number of sets:              4
  Write hit:                   write-back
  Write miss:                  write-allocate
Results:
  Total accesses:       20
  Reads:                10
  Writes:               10
  Hits:                 5
  Misses:               15
  Hit rate:             25.00%
  Writebacks:           4
  Writes to next level: 4
```

## Design
#### Data Structures
- ##### New:
  - cache_config.h: POD struct for cache configuration data. Supports cache configuration inputs and write policies.
  - lookup_result.h: POD struct for cache lookup results, hit/miss status and writeback occurrence.
  - stats.h: POD struct for maintaining cache performance metrics.
- ##### Updates:
  - cache: cache now supports POD input with constructor and POD output printing.

#### Utility:
- ##### New:
  - process_input.h: Utility function to parse input file, extracting cache configuration and memory addresses.



#### Scope
- Configurable write policies for hits and misses.
- POD datatypes for input and output.
