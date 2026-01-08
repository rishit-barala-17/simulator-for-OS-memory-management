# CLI Command Reference

This document lists all supported interactive commands for each simulator mode. Commands are entered at the prompt shown by the compiled executable (e.g., `memsim.exe`).

## mem (memory allocator)
| Command | Description |
|---------|-------------|
| `init <size>` | Initialize physical memory with `<size>` bytes. Example: `init 1024` |
| `set <first\|best\|worst>` | Select allocation policy. Example: `set first_fit` |
| `malloc <size>` | Allocate a block of `<size>` bytes. Example: `malloc 100` |
| `free <id>` | Free an allocated block by id. Example: `free 1` |
| `dump` | Print current memory layout and blocks |
| `stats` | Show allocator statistics and fragmentation info |

## buddy (buddy allocator)
| Command | Description |
|---------|-------------|
| `init <size>` | Initialize buddy allocator with total size (bytes). Example: `init 1024` |
| `alloc <size>` | Allocate a block using the buddy allocator. Example: `alloc 128` |
| `free <addr> <size>` | Free a block by address and size. Example: `free 0x100 128` |
| `dump` | Print buddy free lists and allocations |

## cache (multi-level cache simulator)
| Command | Description |
|---------|-------------|
| `policy <fifo\|lru\|lfu>` | Set replacement policy. Example: `policy lru` |
| `init <l1> <l2> <l3>` | Initialize L1, L2, L3 cache sizes. Example: `init 64 256 1024` |
| `access <addr>` | Access a memory address. Example: `access 0x1A` |
| `stats` | Print cache hit/miss statistics for all levels |

## vm (virtual memory / paging)
| Command | Description |
|---------|-------------|
| `init <pages> <frames> <page_size>` | Initialize paging system. Example: `init 64 16 4096` |
| `access <addr>` | Access a virtual address. Example: `access 1234` |
| `translate <addr>` | Resolve virtual to physical address. Example: `translate 42` |
| `stats` | Print page faults, replacements, and hit/miss rates |

## Global / Utility
| Command | Description |
|---------|-------------|
| `mode <mem\|buddy\|cache\|vm>` | Switch interactive mode. Example: `mode mem` |
| `exit` | Quit the simulator |

## Notes
- Many commands accept either direct mode (after `mode`) or mode-prefixed form (e.g., `init memory 1024`, `init buddy 1024`).
- Use `help` in any mode to see a short command list for that mode.
