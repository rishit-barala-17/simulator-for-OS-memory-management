#include <iostream>
#include <sstream>
#include <string>

#include "01_mem/mem_allocator.h"
#include "02_buddy/buddy_alloc.h"
#include "03_cache/cache_layer.h"
#include "04_vm/virtual_memory.h"

using namespace std;

int main()
{
    MemAllocator* memAlloc = nullptr;
    BuddyAllocator* buddyAlloc = nullptr;
    CacheHierarchy* cache = nullptr;
    VirtualMemory* vm = nullptr;

    ReplacementPolicy cachePolicy = ReplacementPolicy::FIFO;

    string line;

    cout << "Unified Memory Management Simulator CLI\n";
    cout << "Modes: mem | buddy | cache | vm\n";
    cout << "Type: mode <name> to switch, exit to quit\n\n";

    string mode;

    while (true)
    {
        cout << "> ";
        if (!getline(cin, line)) break;

        stringstream ss(line);
        string cmd;
        ss >> cmd;

        if (cmd == "exit")
            break;

        if (cmd == "mode")
        {
            ss >> mode;
            cout << "Switched to mode: " << mode << endl;
            continue;
        }

        /* ================= MEMORY ALLOCATOR ================= */
        if (mode == "mem")
        {
            if (cmd == "help")
            {
                cout << "init <size>\nset <first|best|worst>\nmalloc <size>\nfree <id>\ndump\nstats\n";
            }
            else if (cmd == "init")
            {
                int size; ss >> size;
                delete memAlloc;
                memAlloc = new MemAllocator(size);
                cout << "Memory initialized\n";
            }
            else if (cmd == "set")
            {
                string a; ss >> a;
                if (!memAlloc) { cout << "Not initialized\n"; continue; }
                if (a == "first") memAlloc->setallocator(FIRST_FIT);
                else if (a == "best") memAlloc->setallocator(BEST_FIT);
                else if (a == "worst") memAlloc->setallocator(WORST_FIT);
                else { cout << "Invalid\n"; continue; }
            }
            else if (cmd == "malloc")
            {
                int s; ss >> s;
                if (memAlloc) memAlloc->memoryAlloc(s);
                else cout << "Not initialized\n";
            }
            else if (cmd == "free")
            {
                int id; ss >> id;
                if (memAlloc) memAlloc->freeBlock(id);
                else cout << "Not initialized\n";
            }
            else if (cmd == "dump")
            {
                if (memAlloc) memAlloc->memoryDump();
            }
            else if (cmd == "stats")
            {
                if (memAlloc) memAlloc->memoryInfo();
            }
            else cout << "Invalid command\n";
        }

        /* ================= BUDDY ALLOCATOR ================= */
        else if (mode == "buddy")
        {
            if (cmd == "init")
            {
                int size; ss >> size;
                delete buddyAlloc;
                buddyAlloc = new BuddyAllocator(size);
                cout << "Buddy allocator initialized\n";
            }
            else if (cmd == "alloc")
            {
                int s; ss >> s;
                if (buddyAlloc) buddyAlloc->memoryAlloc(s);
                else cout << "Not initialized\n";
            }
            else if (cmd == "free")
            {
                int addr, size;
                ss >> hex >> addr >> dec >> size;
                if (buddyAlloc) buddyAlloc->freeBlock(addr, size);
                else cout << "Not initialized\n";
            }
            else if (cmd == "dump")
            {
                if (buddyAlloc) buddyAlloc->memoryDump();
            }
            else cout << "Invalid command\n";
        }

        /* ================= CACHE ================= */
        else if (mode == "cache")
        {
            if (cmd == "policy")
            {
                string p; ss >> p;
                if (p == "fifo") cachePolicy = ReplacementPolicy::FIFO;
                else if (p == "lru") cachePolicy = ReplacementPolicy::LRU;
                else if (p == "lfu") cachePolicy = ReplacementPolicy::LFU;
                else { cout << "Invalid\n"; continue; }
            }
            else if (cmd == "init")
            {
                int l1,l2,l3; ss >> l1 >> l2 >> l3;
                delete cache;
                cache = new CacheHierarchy(l1,l2,l3,cachePolicy);
                cout << "Cache initialized\n";
            }
            else if (cmd == "access")
            {
                int addr; ss >> addr;
                if (cache) cache->read(addr);
                else cout << "Not initialized\n";
            }
            else if (cmd == "stats")
            {
                if (cache) cache->printStats();
            }
            else cout << "Invalid command\n";
        }

        /* ================= VIRTUAL MEMORY ================= */
        else if (mode == "vm")
        {
            if (cmd == "init")
            {
                int p,f,ps; ss >> p >> f >> ps;
                delete vm;
                vm = new VirtualMemory(p,f,ps);
                cout << "VM initialized\n";
            }
            else if (cmd == "access")
            {
                int a; ss >> a;
                if (vm) vm->touch(a);
                else cout << "Not initialized\n";
            }
            else if (cmd == "translate")
            {
                int a; ss >> a;
                if (!vm) { cout << "Not initialized\n"; continue; }
                bool fault;
                int phys = vm->resolve(a,fault);
                if (phys != -1)
                    cout << "VA " << a << " -> PA " << phys << " fault=" << fault << endl;
                else
                    cout << "Translation failed\n";
            }
            else if (cmd == "stats")
            {
                if (vm) vm->printStats();
            }
            else cout << "Invalid command\n";
        }

        else
        {
            cout << "Select mode first\n";
        }
    }

    delete memAlloc;
    delete buddyAlloc;
    delete cache;
    delete vm;
    return 0;
}
