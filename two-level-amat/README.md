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



## Cache Hit Ratio derivation
$$HitRate = \frac{UniqueInstructions_{count} \times (Passes_{total} - 1)}{UniqueInstructions_{count} \times Passes_{total}}$$

# Results
## Emulation
### Small program doing some instructions, repeating a few of them. trace_rw1
| Config | Trace | L1 Hit Rate | L2 Hit Rate | AMAT |
|--------|-------|-------------|-------------|------|
| cache_size_increase/small.txt | trace_rw1 | 8.33 % | 72.73 % | 35.17 Cycles |
| cache_size_increase/medium.txt  | trace_rw1 | 75.00 % | 0.00 % | 28.50 Cycles |


### Small number of repeating instructions. trace_rw2 | trace_rw3
| Config | Trace | L1 Hit Rate | L2 Hit Rate | AMAT |
|--------|-------|-------------|-------------|------|
| cache_size_increase/small.txt | trace_rw2 | 9.26 % | 89.80 % | 19.33 Cycles |
| cache_size_increase/medium.txt  | trace_rw2 | 9.26 % | 89.80 % | 19.33 Cycles |
| cache_size_increase/small.txt | trace_rw3 | 91.67 % | 0.00 % | 10.17 Cycles |
| cache_size_increase/medium.txt  | trace_rw3 | 91.67 % | 0.00 % | 10.17 Cycles |


## Phase locality. Multiple working sets in tandem. trace_rw4
| Config | Trace | L1 Hit Rate | L2 Hit Rate | AMAT |
|--------|-------|-------------|-------------|------|
| cache_size_increase/small.txt | trace_rw4 | 50.00 % | 66.67 % | 22.67 Cycles |
| cache_size_increase/medium.txt  | trace_rw4 | 83.33 % | 0.00 % | 19.33 Cycles |

## Sequentially ordered instructions. trace_rw5 | trace_rw6 | trace_rw7

## Cache Analysis
Emulating a cache in C++ gives us a good baseline to see how modifications to a Cache change performance. Though we're only measuring a single metric for any given configuration, the Average Memory Access Time (AMAT), we can establish some ideas about how to optimize a cache's performance across multiple working sets. We will be establishing a 'benchmark' for this 2-level cache emulator, identifying optimal combinations of configuration. I've also included some analagous descriptors comparing these modifications to a bookshelf. The values we will be looking at are as follows:
- Cache Size *(The size of the bookshelf)*
- Associativity *(The number of slots on a shelf)*
- Block Size *(The size of the slot a book will fit in)*
- Write Hit Policy *(If put a book into the shelf, what do we do if it's already there?)*
- Write Miss Policy *(If we put a book into the shelf, what do we do if it's not there?)*

An important absence in the above is the number of sets in each cache, analagous to the number of shelves in a bookshelf. This is because the number of sets is derived, not configured, with the following formula:
$$NumSets = \frac{CacheSize}{BlockSize \times Associativity} $$

### Cache Size Changes on Sequential Working set
| Config | Trace | L1 Hit Rate | L2 Hit Rate | AMAT |
|--------|-------|-------------|-------------|------|
| cache_size_increase/small.txt | trace_rw5 | 0.00 % | 0.00 % | 111.00 Cycles |
| cache_size_increase/medium.txt  | trace_rw5 | 0.00 % | 50.00 % | 61.00 Cycles |
| cache_size_increase/large.txt  | trace_rw5 | 50.00 % | 0.00 % | 56.00 Cycles |
| cache_size_increase/gigantic.txt  | trace_rw5 | 50.00 % | 0.00 % | 56.00 Cycles |
| cache_size_increase/gigantic.txt  | trace_rw6 | 87.50 % | 0.00 % | 14.75 Cycles |

The small.txt cache config tests an exceedingly small L1/L2 cache size of (L1 128 / L2 512) and has a 100% miss rate. We can derive this behavior by demonstrating the number of sets in L1 and L2 small.txt as:
$$ Sets_{L1} = \frac{128}{16 \times 4}$$
$$ Sets_{L1} = 2 $$

$$ Sets_{L2} = \frac{512}{32 \times 8}$$
$$ Sets_{L2} = 2 $$

This configuration has 2 sets with 4 ways each in both L1 and L2, a total of 16 unique blocks in the entire Cache. At the time of the 17th sequential instruction, the 1st is guaranteed to be kicked out of the cache. On every instruction, we are paying the access penalty of missing L1 (1 Cycle), missing L2 (10 Cycles), and accessing RAM (100 Cycles) every instruction.

When increasing cache size in medium.txt (L1 1024 / L2 4096), we see substantial AMAT improvement from 111.00 Cycles to 61.00 Cycles with L2 Cache hits explaining the difference. We can quickly verify this by calculating the number of sets and total possible addresses:
$$ Sets_{L1} = \frac{1024}{16 \times 4}$$
$$ Sets_{L1} = 16 $$

$$ Sets_{L2} = \frac{4096}{16 \times 4}$$
$$ Sets_{L2} = 64 $$

In theory, the L1 Cache now has 16 sets with 4 ways each, a total of 48 unique blocks. This has the potential to hold all 40 unique instructions, so why are we seeing a 0% hitrate in L1 and 50% hitrate in L2 here? To start, the addresses in trace_rw5.txt increment by 0x20 every time, 32 bytes, or 2 blocks. Since our address calculation is based on a number of bits equal to the number of sets, we must look at our addresses and what sets they map to:
|Address|Bits [7:4]|Set|
|------|------|---| 
| 0x000 |	0000	| 0 | 
| 0x020	| 0010 | 	2
| 0x040	| 0100	| 4
| 0x060	| 0110 | 	6
| 0x080	| 1000	| 8
| 0x0A0	| 1010 |  	10
| 0x0C0	| 1100 | 	12 | 
| 0x0E0	| 1110	| 14 | 
| 0x100	| 0000	| 0 | 

The 9th instruction returns to Set 0. This happens on the 9th, 17th, 25th, and 33rd instructions, a total of 5 times. If we were to look at our cache before the final 8 instructions, you would see 8 sets of 4 ways completely full with 32 unique blocks. The final 8 instructions are the inserted, missing and overwriting the least recently used item in each set. This then cascades on the second pass through the same 40 unique instructions.

We can conclude that the capacity to hold a working set does not give confidence of a high performance.

While the L2 Cache's only change is size, the key reason it shows a 50% hitrate compared to L1's 0% hitrate is due to the derived number of sets, a total of 64 sets. The same looping behavior can still exist, but would happen on 0x400 instead of 0x100.

We can conclude that even if a cache has sufficient size to contain a working set, derived number of sets is important to AMAT performance. Since this is derived, we then can say that the Block Size and Associativity are of high importance.

I want to verify the 50% hit rate here, so let's look at the number of passes. trace_rw5 contains 40 unique instructions. We can calculate the number of passes in medium.txt by using the formula: 
- Hit Rate = N ( k - 1 ) / N * k
Where N is the number of unique instructions and k is the number of passes.

$$.50 = (40 * (K - 1)) / (40 * k)$$
$$.50 = (k - 1) / k$$
$$.50 * k = k - 1$$
$$k - .50 * k = 1$$
$$k * .50 = 1$$
$$k = 2 $$

Since K = 2, we've confirmed this is running 2 passes, so the working set has 40 cold misses the first time, then 40 hits the second in L2.

Further increases to size in large.txt (L1 4096 / L2 16384) and gigantic.txt (L1 16384 / L2 65536), we shift these hit savings into L1 cache and slightly improve cycle savings but with a much smaller improvement from 61.00 Cycles to 56.00 Cycles. Since we've verified that medium.txt L2 Cache Size of 4096 is large enough to contain the working set, we know that large.txt L1 Cache Size of 4096 is large enough to do so. AMAT savings therefore come from the L1 Cache Size meeting the working sets threshold in both large.txt and gigantic.txt.

An important note is that AMAT is identical between large.txt and gigantic.txt despite the latter being 4 times larger. This is because the L1 Cache Size already encapsulates the working set, so increasing its size further doesn't provide any benefit. A good analogy would be quadrupling your 100 meter track so you can run a faster 100 meter race.

To investigate further, we ran trace_rw6 with the gigantic.txt config. trace_rw6 has the same sequential instructions as trace_rw5 but repeated multiple times, so we can show that AMAT scales with the number of passes and not cache size.
$$Hit Rate = N ( k - 1 ) / N * k$$
$$.875 = ( 40 * (K - 1) )/ 40 * k$$
$$.875 = (k - 1) / k$$
$$.875 * k = k - 1$$
$$k (1 - .875) = 1$$
$$k * .125 = 1$$
$$k = 8$$

In both calculations, we clearly end up with K = 1 / (1 - hit rate). As hit rate approaches 1 (100%), the denominator approaches 0, and K increases. We can flip this and state that as K increases, the hit rate approaches 100%.

We can conclude that we just need a cache of sufficient size for sequential working sets. Further cache size increases only yield AMAT benefits if they cause a cache level to hit the working set's threshold.

### Write Policy Data
#### trace_rw7
| Config | Trace | L1 Hit Rate | L2 Hit Rate | AMAT |
|--------|-------|-------------|-------------|------|
| write_policies/wb_wa_small | trace_rw7 | 0.00 % | 0 %  | 111.00 Cycles |
| write_policies/wt_wa_small | trace_rw7 | 0.00 % | 0 %  | 111.00 Cycles |
| write_policies/wt_nwa_small | trace_rw7 | 0.00 % | 0 %  | 111.00 Cycles |
| write_policies/wb_wa_med | trace_rw7 | 0.00 % | 87.50 %  | 23.50 Cycles |
| write_policies/wt_wa_med | trace_rw7 | 0.00 % | 87.50 %  | 23.50 Cycles |
| write_policies/wt_nwa_med | trace_rw7 | 43.75 % | 0 %  | 67.25 Cycles |
| write_policies/wb_wa | trace_rw7 | 0.00 % | 87.50 % | 23.50 Cycles |
| write_policies/wt_wa | trace_rw7 | 0.00 % | 87.50 %  | 23.50 Cycles |
| write_policies/wt_nwa | trace_rw7 | 0.00 % | 43.75 %  | 67.25 Cycles |
| write_policies/wt_nwa_large | trace_rw7 | 43.75 % | 0.00 %  | 62.88 Cycles |





### Write Policy Analysis
We've established that for this sequential set, cache size increases only provide AMAT benefits when they push cache levels over the working set thresholds. This axiom holds with varying write policies - the smallest caches in a sequential working set have the maximum AMAT of 111.00 Cycles, and while larger caches show clear improvement, further size increases do nothing to improve AMAT.

To understand why *NWA* write miss behavior in *wt_nwa_med*, *wt_nwa*, and *wt_nwa_large*, which share an L2 HitRate of 43.75% and AMAT of 23.50 Cycles, we need to understand the working set. Because this is a sequential working set with 40 unique instructions (20 Reads, 20 Writes), our HitRate with *NWA* policy is capped at 
$$\frac{20}{40} \times \frac{(K - 1)}{K} $$

Since we know 320 instructions of 40 unique addresses is 8 passes, we can plug this in to find:

$$\frac{1}{2} \times \frac{8 - 1}{8}$$
$$0.5 \times 0.875  = .4375 $$
We can also derive our expected hit rate with the following:
$$ Cache_{misses} = Write_{misses} + Read_{misses} $$
$$ MissRate = (Cache_{misses} / Cache_{accesses}) $$
$$ HitRate = 1 - MissRate $$

Writes will not update the cache under *NWA*, so we are guaranteed the first 20 reads are cold misses, leaving us with:

$$ Cache_{misses} = 160 + 20 = 180 $$
$$ MissRate = \frac{180}{320} = .5625 $$
$$ HitRate = 1 - .5625 = .4375 = 43.75 \% $$

Our derived HitRate exactly matches our emulator's achieved hitrate for both *wt_nwa* and *wt_nwa_large* exactly. Our expectations of an *NWA* write policy align with these observations; *NWA* is intended to avoid polluting the cache with write data that won't be re-read. The following section will explore this further by varying write policies between L1 and L2 caches.

#### trace_rw7 continued
| Config | Trace | L1 Hit Rate | L2 Hit Rate | AMAT |
|--------|-------|-------------|-------------|------|
| write_policies/wtnwa_l1_wbwa_l2_large | trace_rw7 | 43.75 % | 77.78 %  | 19.12 Cycles |
| write_policies/wtwa_l1_wbwa_l2_large | trace_rw7 | 87.50 % | 0 %  | 14.75 Cycles |
| write_policies/wbwa_l1_wtnwa_l2_large | trace_rw7 | 87.50 % | 0 %  | 14.75 Cycles |
| write_policies/wbwa_l1_wtwa_l2_large | trace_rw7 | 87.50 % | 0 %  | 14.75 Cycles |
| write_policies/wt_nwa_large | trace_rw7 | 43.75 % | 0.00 %  | 62.88 Cycles |


Configuring an L1 *WA* write policy shows clear performance benefits, with a consistent 14.75 Cycle AMAT. This cycle time only changes when the L1 Cache adopts an *NWA* Write Policy, increasing AMAT to 19.12 Cycles. 

The reasoning is clear when you look at L1 and L2 HitRates run with *wtnwa_l1_wbwa_l2_large*. The L1 HitRate drops to 43.75 % as Write Misses no longer allocate in L1, instead falling through to L2. The L2 previously had 40 accesses from cold misses, but now has a 77.78% hit rate across 180 accesses. The same can be expanded into *wt_nwa_large*, as the policy now also gates Write Misses out of L2 and straight to memory, resulting in the much higher penalty and 62.88 AMAT.

In a single sequential working set, the L1 cache should be configured with a Write-Allocate write policy.