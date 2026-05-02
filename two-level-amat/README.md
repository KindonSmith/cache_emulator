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


### Config Matrix

| Config | Trace | L1 Hit Rate | L2 Hit Rate | AMAT |
|--------|-------|-------------|-------------|------|
| default | trace_rw1 | | | |
| default | trace_rw2 | | | |
| default | trace_rw3 | | | |
| tinyl1 | trace_rw1 | | | |
| tinyl1 | trace_rw2 | | | |
| tinyl1 | trace_rw3 | | | |
| bigl2 | trace_rw1 | | | |
| bigl2 | trace_rw2 | | | |
| bigl2 | trace_rw3 | | | |

## Key Observations


- **trace_rw2 (L1 thrashing):** ...
- **trace_rw3 (small working set):** ...
