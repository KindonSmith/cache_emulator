# Direct-mapped Cache

A direct-mapped cache emulator where each instruction maps directly to a single address.

## Build
bash cd src
g++ main.cpp -o dm-cache.exe

## Use
./dm-cache.exe <input_file>

## Spec 
**Cache configuration:**
- Cache size: 1024 bytes
- Block size: 32 bytes
- Address size: 32-bit

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