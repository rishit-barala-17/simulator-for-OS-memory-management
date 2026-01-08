# Design and Documentation

## 1. Overview

### Purpose
This repository provides a user‑space C++ simulator that models core operating‑system memory management concepts for laboratory use in an OS course.

### Educational goals
- Allow hands‑on exploration of contiguous allocation policies (First Fit, Best Fit, Worst Fit), the buddy allocator, multilevel cache behavior, and paging with FIFO replacement.
- Make allocation decisions, fragmentation effects, cache performance, and page‑fault handling visible and repeatable.

### Scope of simulation
- The project is a software simulator implemented in C++ using in‑memory data structures and deterministic algorithms. It is not kernel code and does not simulate hardware timing or concurrency. The CLI invokes module APIs and does not embed core simulation logic.

---

## 2. Project Structure

Repository layout

```
src/
├─ main.cpp
├─ 01_mem/
│  ├─ mem_Allocator.cpp
│  └─ mem_allocator.h
├─ 02_buddy/
│  ├─ Buddy_allocator.cpp
│  └─ buddy_alloc.h
├─ 03_cache/
│  ├─ Cache.cpp
│  └─ cache_layer.h
└─ 04_vm/
   ├─ Virtual_memory.cpp
   └─ virtual_memory.h


docs/
├─ 01_mem/
   ├─mem_allocator_output.png
   └─mem_allocator_output.txt
├─ 02_buddy/
   ├─buddy_output.png
   └─buddy_output.txt
├─ 03_cache/
   ├─cache_output.png
   └─cache_output.txt
├─ 04_vm/
   ├─vm_output.png
   └─vm_output.txt
├─ cli/
│  ├─ 01_mode_mem/
│  ├─ 02_mode_buddy/
│  ├─ 03_mode_cache/
│  └─ 04_mode_vm/
├─ cli_cmd_reference.md
├─ demo.mp4 (demo video)
└─ design.md

tests/
├─ mem_tests.cpp
├─ buddy_tests.cpp
├─ cache_tests.cpp
└─ vm_tests.cpp
```

Module responsibilities
- CLI (src/main.cpp): parse user commands, select or auto‑select module modes, invoke module APIs, manage module lifetimes, and report initialization/state errors. The CLI contains no allocation, cache, or VM algorithms; it orchestrates calls to modules.
- Physical allocator (01_mem): contiguous allocation strategies, block metadata, splitting/coalescing, and fragmentation/statistics reporting.
- Buddy allocator (02_buddy): power‑of‑two buddy allocation with free lists and coalescing rules.
- Cache simulator (03_cache): multilevel cache simulation (L1/L2/L3), replacement policies, and cache statistics.
- Virtual memory (04_vm): paging, page table management, FIFO page replacement, virtual→physical translation, and VM statistics.

---

## 3. Memory Layout and Assumptions

### Address representation
- Addresses are integers within the simulator. Module outputs commonly format addresses in hexadecimal for readability.

### Memory abstraction
- Physical memory is modeled as a contiguous address space with a configurable total size. Allocator modules maintain explicit metadata per block (start address, size, free/used, block id).

### Simplifications and assumptions
- Single logical address space (single‑process model).
- Single‑threaded execution; no synchronization or concurrency behavior is modeled.
- No hardware timing: the simulator records logical events (hits, misses, faults) but does not model latency or cycles.

---

## 4. Physical Memory Allocation Strategies

### Algorithms implemented
- First Fit: choose the first free block large enough for the request.
- Best Fit: choose the smallest free block that fits.
- Worst Fit: choose the largest free block.

### Block splitting
- When an allocation request fits a larger free block, the block is split into a used portion and a remaining free portion. Metadata is updated to reflect the new blocks.

### Free block coalescing
- On deallocation the allocator merges adjacent free blocks based on block start addresses and sizes to reduce external fragmentation.

### Fragmentation metrics (provided by the module)
- Internal fragmentation — unused bytes inside allocated blocks.
- External fragmentation — unusable free space due to fragmentation.
- Memory utilization — used bytes divided by total memory.
- Allocation success rate — fraction of allocations that succeed.

---

## 5. Buddy Memory Allocation System

### Power‑of‑two design
- The buddy allocator requires the total memory size to be a power of two. Requested sizes are rounded up to the nearest power of two.

### Free list organization
- Free lists are maintained per block size class (2^k). Each free list holds blocks of the same size.

### Allocation flow
- Allocate from the smallest size class that can satisfy the request. If necessary, split a larger block recursively until a block of the requested size is produced.

### Coalescing rules
- When freeing a block, the buddy address is computed (XOR with block size). Coalescing is performed only when:
  - both buddies are free,
  - both are of the same block size,
  - and the free exactly matches the expected start address and size.

### Rationale for strict coalescing
- Coalescing depends on precise invariants (addresses and sizes). If a free operation does not meet these invariants (e.g., incorrect address/size or partial free), coalescing is not performed to preserve free‑list consistency.

---

## 6. Cache Hierarchy and Replacement Policies

### Cache levels
- The simulator models a multilevel cache: L1 → L2 → L3. Each level maintains explicit cache lines and capacity set at initialization.

### Replacement policies
- FIFO — evict in insertion order.
- LRU — evict the least recently used line.
- LFU — evict the least frequently used line.

### Hit/miss propagation
- On access, levels are checked in order (L1, then L2, then L3). A miss at one level causes an access at the next level; an L3 miss corresponds to a memory access. Hits and misses are counted per level.

### Statistics collected
- Per‑level hit and miss counts, and aggregate ratios. The cache module exposes a printStats API for these values.

### Cache hierarchy diagram
```text
[L1 Cache]
   |
   v
[L2 Cache]
   |
   v
[L3 Cache]
   |
   v
[Main Memory]
```

---

## 7. Virtual Memory Model

### Page table structure
- The simulator maintains a page table entry per virtual page containing:
  - valid bit
  - frame number (if valid)

### Configurable parameters
- Number of pages, number of frames, and page size are set at VM initialization.

### Page fault handling
- An access to an invalid page triggers a page fault. The VM loads the required page into a frame and updates the page table.

### Replacement policy
- FIFO page replacement selects a victim frame when necessary.

### Statistics tracked
- Page hits and page faults, plus translation metrics reported by the VM module.

### Page table diagram
```text
+-------------------------+
| Page Table (array)      |
+-------------------------+
| PTE[0] | valid | frame  |
| PTE[1] | valid | frame  |
| PTE[2] | valid | frame  |
|  ...                     |
+-------------------------+
```

---

## 8. Address Translation Flow

### Step‑by‑step sequence implemented
1. Split virtual address into page index and offset (using page size).
2. Look up page table entry:
   - If valid: compute physical address = frame * page_size + offset.
   - If invalid: page fault → VM handles fault (FIFO replacement as required) → update page table → compute physical address.
3. Optionally pass the physical address to the cache subsystem for further simulation (L1 → L2 → L3).
4. Complete the memory access and update relevant statistics.

### Translation diagram
```text
Virtual Address
     |
     v
[Page Table Lookup] --(valid?)--> Yes --> Physical Address
     |                                    |
     No (fault)                            v
     |                                [Cache L1]
     v                                    |
[Page Fault Handler]                      v
     |                                [Cache L2]
     v                                    |
[Load page, update PTE]                   v
     |                                [Cache L3]
     v                                    |
Physical Address ----------------------> [Main Memory]
```

---

## 9. Integration Between Components

### Module independence
- Each module implements its own logic and exposes a clear API. Modules do not embed the logic of other modules; integration happens when the CLI or a test invokes modules in sequence.

### Typical combined operation
- Example flow when VM and cache are exercised together:
  - CLI requests a VM access → VM resolves virtual→physical (may cause page fault) → CLI or test passes physical address to cache read API → cache simulation updates hit/miss counters accordingly.

### Role of the CLI
- The CLI parses user commands (both mode‑based and specification‑style), performs argument validation, initializes modules, and calls their APIs. It is responsible for orchestration and state management, not the underlying simulation algorithms.

---

## 10. Testing Methodology

### Test philosophy
- Tests are deterministic and non‑interactive. They run fixed workloads to produce repeatable traces for verification and grading.

### Non‑interactive workloads
- Each module has a corresponding test program in tests/ that drives a predefined sequence of operations and prints results to the terminal.

### Validation criteria
- Tests exercise:
  - allocation correctness (addresses, sizes, ids),
  - splitting and coalescing logic for the contiguous allocator,
  - buddy allocator splitting/coalescing invariants,
  - cache hit/miss propagation and counters,
  - page faults and translation correctness.

### Trace capture for verification
- Test outputs are captured in docs/ as authoritative traces used for grading.

---

## 11. Limitations and Simplifications

This section states the explicit design choices and constraints of the simulator in a balanced way:

- Single‑process model: The simulator represents one logical address space. Process isolation and multi‑process interactions are not modeled.
- Single‑threaded execution: All modules are exercised sequentially; synchronization, locks, and concurrent races are outside the scope.
- No hardware timing: The project records logical events (hits, misses, faults) but does not simulate latencies, bus arbitration, or cycle counts.
- User‑space simulation: All behavior is implemented in C++ data structures. There is no kernel code, device drivers, or direct hardware manipulation.
- These choices keep the implementation focused, deterministic, and suitable for lab assignments where the goal is to study algorithmic behavior rather than to model low‑level hardware or parallel execution.

---

## 12. Conclusion

### Summary
- This simulator provides implemented, verified modules for contiguous allocation strategies, a buddy allocator, a multilevel cache with multiple replacement policies, and a paging virtual memory system with FIFO replacement. The CLI coordinates module usage without embedding core simulation logic.

### Correctness and completeness
- The document describes implemented features only. Modules are exercised with deterministic tests and their outputs are stored in docs/ to support verification and grading.