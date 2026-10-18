# Virtual Memory Page Replacement Simulator

A C++ simulator for virtual memory management evaluating LRU and Second Chance page replacement algorithms using a Hashed Page Table.

## Features

- **Algorithms Supported:**
  - **LRU (Least Recently Used):** Removes the page that hasn't been used for the longest time.
  - **Second Chance (Clock):** Uses reference bits to give recently used pages a second chance before replacement.
- **Data Structures:**
  - Custom Hashtable with chaining for the Hashed Page Table
  - `std::unordered_map` for fast $O(1)$ lookups
- **Trace Processing:** Reads real memory access traces (`bzip.trace`, `gcc.trace`).
- **Statistics:** Calculates total memory accesses, Page Faults, Disk Reads, and Disk Writes (handling dirty pages).

## Build

### Requirements
* C++11 (or higher)
* CMake 3.10+

### Build Steps

```bash
mkdir build && cd build
cmake ..
make
```
## Usage

Run the simulator from the project root directory so it can access the trace files:

```bash
./build/vm_simulator <algorithm> <num_frames> <q_max_chain> <max_references>
```

### Command-Line Arguments

| Parameter | Description |
| :--- | :--- |
| `<algorithm>` | `1` for **LRU**, `2` for **Second Chance** |
| `<num_frames>` | Number of allocated physical memory frames (e.g., `100`, `500`, `1000`) |
| `<q_max_chain>` | Maximum hash bucket chain threshold (e.g., `5`) |
| `<max_references>` | Number of references to read (`0` or leave blank for all) |
