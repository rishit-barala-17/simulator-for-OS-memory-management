# Demo Video

Watch a short demo of the simulator :

<video controls width="720" preload="metadata">
  <source src="docs/demo.mp4" type="video/mp4">
  Your browser does not support the video tag. Download the video: [Demo video](docs/demo.mp4)
</video>

# OS Memory Management Simulator

## 1. Project Title
OS Memory Management Simulator

## 2. Brief Description
User‑space C++ simulator for core memory management concepts used in an operating‑systems laboratory. Provides interactive CLI and non‑interactive tests to exercise implemented modules.

## 3. Features
- Physical contiguous allocator (First Fit, Best Fit, Worst Fit)  
- Buddy memory allocator (power‑of‑two)  
- Multilevel cache simulator (L1, L2, L3) with FIFO, LRU, LFU policies  
- Paging virtual memory with page faults and FIFO replacement  
- Unified CLI for manual use; deterministic non‑interactive tests for verification

## 4. Project Structure
- src/  
  - main.cpp  
  - 01_mem/ (physical allocator)  
  - 02_buddy/ (buddy allocator)  
  - 03_cache/ (cache simulator)  
  - 04_vm/ (virtual memory)  
- tests/ (non‑interactive test programs)  
- docs/  
  - 01_mem/mem_allocator_output.txt
  - 02_buddy/buddy_output.txt
  - 03_cache/cache_output.txt
  - 04_vm/vm_output.txt
  - cli/
    - 01_mode_mem/
    - 02_mode_buddy/
    - 03_mode_cache/
    - 04_mode_vm/
  - cli_cmd_reference.md 
  - demo.mp4 (demo video)
  - design.md 

## 5. Build Instructions

Windows (MinGW / PowerShell)
```powershell
# from repository root
g++ -std=c++17 -O2 src\main.cpp src\01_mem\mem_Allocator.cpp src\02_buddy\Buddy_allocator.cpp src\03_cache\Cache.cpp src\04_vm\Virtual_memory.cpp -o memsim.exe
.\memsim.exe
```

Linux / macOS
```bash
# from repository root
g++ -std=c++17 -O2 src/main.cpp src/01_mem/mem_Allocator.cpp src/02_buddy/Buddy_allocator.cpp src/03_cache/Cache.cpp src/04_vm/Virtual_memory.cpp -o memsim
./memsim
```

## 6. How to Run
- Run the compiled executable (memsim or memsim.exe).  
- Use the CLI to initialize modules, run allocations, exercise cache/VM behavior, or invoke test programs.

## 7. Quick CLI Example

**Memory Allocator:**
```
> init memory 1024
> set allocator first_fit
> malloc 256
> malloc 512
> malloc 100
> free 1
> dump
> stats
```

**Buddy Allocator:**
```
> init buddy 2048
> alloc 256
> alloc 512
> free 0x0 256
> dump
```

**Cache Simulator:**
```
> policy lru
> init 64 256 1024
> access 0x100
> access 0x200
> access 0x100
> stats
```

**Virtual Memory:**
```
> init vm 64 16 4096
> access 1000
> access 2000
> translate 1000
> stats
```

## 8. Testing
- Tests are under tests/ (mem_tests.cpp, buddy_tests.cpp, cache_tests.cpp, vm_tests.cpp).  
- Build a test by compiling the test source with the module sources, e.g. (Linux):
```bash
g++ -std=c++17 -O2 tests/mem_tests.cpp src/01_mem/mem_Allocator.cpp -o mem_tests
./mem_tests
```
- Captured, deterministic outputs are stored in docs/*_output.txt for verification.

## 9. Documentation Reference
- **CLI Command Reference**: See [docs/cli_cmd_reference.md](docs/cli_cmd_reference.md) for complete command documentation.  
- Detailed design and module descriptions: docs/design.md  
- Test traces and logs: docs/*_output.txt

## 10. Notes / Assumptions
- Simulator runs entirely in user space; it is not kernel code.  
- Single‑process, single‑threaded, deterministic simulation.  
- CLI parses commands and invokes module APIs; core algorithms remain inside their modules.  
- Follow docs/design.md for design details and the recorded test traces for verified behavior.