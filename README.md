# Cache Emulator

Cache emulation progression series; starting with L1 direct-mapping and progressing to dynamic configuration, write policies, multi-level hierarchies, and stateful AMAT tracking.

## Overview

Built as prep for Georgia Tech's OMSCS program, focusing on C++ systems programming and HPCA.

### Projects
| # | Project | Description |
|---|---------|-------------|
| 1 | [direct-mapped-cache](./direct-mapped-cache) | Fixed size, direct-mapped cache tracking accesses, hits, and misses. |
| 2 | [4-way-set-associative-lru](./4-way-set-associative-lru) | Set-associative ways, LRU replacement |
| 3 | [configurable-cache-set-associative-lru](./configurable-cache-set-associative-lru) | CLI runtime configuration |
| 4 | [write-policies](./write-policies) | Write-back / write-through + write-allocate / no-write-allocate policies, dirty bit tracking |
| 5 | [two-level-amat](./two-level-amat) | ... |

### Prerequisites
- g++ / GCC
- Make

### Build and Run
Projects build independently, so please see each project's respective README. 


### Author
Kindon