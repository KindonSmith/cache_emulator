# Two-Level Cache Simulator + AMAT

Simulates a two-level (L1 + L2) cache hierarchy with configurable write policies and computes **Average Memory Access Time (AMAT)**.

## Build

```bash
cd src
g++ main.cpp classes/cache_system.cpp -o amat.exe
```

## Usage

```bash
./amat.exe --config config/<config_file>.txt --input input/<trace_file>.txt
```

## Configuration Format
#### Write policies
Valid:
- wb + wa
- wt + wa
- wt + nwa

Invalid:
- wb + nwa - these policies are fundamentally opposed to each other


Two caches, each must include 6 relevant configs
- size
- block size
- assoc
- write hit policy
- write miss policy
- and hit time

```
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

## Trace Format

```
R 0x1000
W 0x1020
R 0x1040
```

## AMAT Formula

$$AMAT = HitTime_{L1} + MissRate_{L1} \times (HitTime_{L2} + MissRate_{L2} \times MemoryAccessTime)$$

Where $MissRate_{L2}$ is the **local** miss rate (L2 misses / L2 accesses).

## Results
### Working set optimization.

#### Cache Size Comparison across multiple working sets
##### Small dynamic working set, trace_rw1. Emulates a small program doing some instructions, repeating a few of them
| Config | Trace | L1 Hit Rate | L2 Hit Rate | AMAT |
|--------|-------|-------------|-------------|------|
| cache_size_increase/small.txt | trace_rw1 | 8.33 % | 72.73 % | 35.17 Cycles |
| cache_size_increase/medium.txt  | trace_rw1 | 75.00 % | 0.00 % | 28.50 Cycles |

##### Tiny working set, trace_rw2 and trace_rw3. Emulates a small number of repeating instructions
| Config | Trace | L1 Hit Rate | L2 Hit Rate | AMAT |
|--------|-------|-------------|-------------|------|
| cache_size_increase/small.txt | trace_rw2 | 9.26 % | 89.80 % | 19.33 Cycles |
| cache_size_increase/medium.txt  | trace_rw2 | 9.26 % | 89.80 % | 19.33 Cycles |
| cache_size_increase/small.txt | trace_rw3 | 91.67 % | 0.00 % | 10.17 Cycles |
| cache_size_increase/medium.txt  | trace_rw3 | 91.67 % | 0.00 % | 10.17 Cycles |

##### Phase locality. Emulates multiple working sets in tandem.
| Config | Trace | L1 Hit Rate | L2 Hit Rate | AMAT |
|--------|-------|-------------|-------------|------|
| cache_size_increase/small.txt | trace_rw4 | 50.00 % | 66.67 % | 22.67 Cycles |
| cache_size_increase/medium.txt  | trace_rw4 | 83.33 % | 0.00 % | 19.33 Cycles |


##### Sequential. Emulates a sequence of instructions in order.
| Config | Trace | L1 Hit Rate | L2 Hit Rate | AMAT |
|--------|-------|-------------|-------------|------|
| cache_size_increase/small.txt | trace_rw5 | 0.00 % | 0.00 % | 111.00 Cycles |
| cache_size_increase/medium.txt  | trace_rw5 | 0.00 % | 50.00 % | 61.00 Cycles |
| cache_size_increase/large.txt  | trace_rw5 | 50.00 % | 0.00 % | 56.00 Cycles |
| cache_size_increase/gigantic.txt  | trace_rw5 | 50.00 % | 0.00 % | 56.00 Cycles |
| cache_size_increase/gigantic.txt  | trace_rw6 | 87.50 % | 0.00 % | 14.75 Cycles |


The small.txt cache config tests an exceedingly small L1/L2 cache size of (L1 128 / L2 512) and has a 100% miss rate. As cache size decreases towards base data sizes, it becomes less useful. In this sequential test, we are never able to hold enough to hit with small.txt cache sizing. We are paying the access penalty of missing L1 (1 Cycle), missing L2 (10 Cycles), and accessing RAM (100 Cycles) every instruction.

When increasing cache size in medium.txt (L1 1024 / L2 4096), we see substantial AMAT improvement from 111.00 Cycles to 61.00 Cycles with L2 Cache hits explaining the difference. We can conclude that the medium.txt L2 Cache Size of 4096 is large enough to contain the working set.

Since we're seeing exactly 50% hit rate here, we can verify number of passes. trace_rw5 contains 40 unique instructions. We can calculate the number of passes in medium.txt by using the formula: 
- Hit Rate = N ( k - 1 ) / N * k
Where N is the number of unique instructions and k is the number of passes.

.50 = (40 * (K - 1)) / (40 * k)
.50 = (k - 1) / k
.50 * k = k - 1
k - .50 * k = 1
k * .50 = 1
k = 2 

Since K = 2, we've confirmed this is running 2 passes, so the working set has 40 cold misses the first time, then 40 hits the second in L2.

Further increases to size in large.txt (L1 4096 / L2 16384) and gigantic.txt (L1 16384 / L2 65536), we shift these hit savings into L1 cache and slightly improve cycle savings but with a much smaller improvement from 61.00 Cycles to 56.00 Cycles. Since we've verified that medium.txt L2 Cache Size of 4096 is large enough to contain the working set, we know that large.txt L1 Cache Size of 4096 is large enough to do so. AMAT savings therefore come from the L1 Cache Size meeting the working sets threshold in both large.txt and gigantic.txt.

An important note is that AMAT is identical between large.txt and gigantic.txt despite the latter being 4 times larger. This is because the L1 Cache Size already encapsulates the working set, so increasing its size further doesn't provide any benefit. A good analogy would be quadrupling your 100 meter track so you can run a faster 100 meter race.

To investigate further, we ran trace_rw6 with the gigantic.txt config. trace_rw6 has the same sequential instructions as trace_rw5 but repeated multiple times, so we can show that AMAT scales with the number of passes and not cache size.
- Hit Rate = N ( k - 1 ) / N * k
.875 = ( 40 * (K - 1) )/ 40 * k
.875 = (k - 1) / k
.875 * k = k - 1
k (1 - .875) = 1
k * .125 = 1
k = 8

In both calculations, we clearly end up with K = 1 / (1 - hit rate). As hit rate approaches 1 (100%), the denominator approaches 0, and K increases. We can flip this and state that as K increases, the hit rate approaches 100%.

We can conclude that we just need a cache of sufficient size for sequential working sets. Further cache size increases only yield AMAT benefits if they cause a cache level to hit the working set's threshold.


## Key Observations


