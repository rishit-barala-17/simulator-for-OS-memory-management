#include <iostream>
#include <sstream>
#include <string>
#include <vector>

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

    cout << "Unified Memory Management Simulator CLI\n";
    cout << "Modes: mem | buddy | cache | vm\n";
    cout << "Type: mode <name> to switch, exit to quit\n\n";

    string mode; // current interactive mode: "mem", "buddy", "cache", "vm"

    string line;
    while (true)
    {
        cout << "> ";
        if (!getline(cin, line)) break;

        // tokenize input
        stringstream parser(line);
        vector<string> tokens;
        string tok;
        while (parser >> tok) tokens.push_back(tok);
        if (tokens.empty()) continue;

        // default: first token is command, rest are args
        string cmd = tokens[0];
        // build args stream normally from tokens[1..]
        auto build_args = [&](size_t start=1) {
            string s;
            for (size_t i = start; i < tokens.size(); ++i) {
                if (i > start) s += " ";
                s += tokens[i];
            }
            return stringstream(s);
        };
        stringstream ss = build_args(1);

        // Global commands
        if (cmd == "exit") break;

        if (cmd == "mode") {
            if (tokens.size() < 2) {
                cout << "Usage: mode <mem|buddy|cache|vm>\n";
                continue;
            }
            string m = tokens[1];
            if (m == "mem" || m == "memory") { mode = "mem"; cout << "Switched to mode: memory\n"; }
            else if (m == "buddy") { mode = "buddy"; cout << "Switched to mode: buddy\n"; }
            else if (m == "cache") { mode = "cache"; cout << "Switched to mode: cache\n"; }
            else if (m == "vm") { mode = "vm"; cout << "Switched to mode: vm\n"; }
            else { cout << "Invalid mode\n"; }
            continue;
        }
        // Special command mappings to modes
        if (cmd == "init" && tokens.size() >= 2) {
            string sub = tokens[1];
            if (sub == "memory") { mode = "mem"; cmd = "init"; ss = build_args(2); }
            else if (sub == "buddy") { mode = "buddy"; cmd = "init"; ss = build_args(2); }
            else if (sub == "cache") { mode = "cache"; cmd = "init"; ss = build_args(2); }
            else if (sub == "vm") { mode = "vm"; cmd = "init"; ss = build_args(2); }
        }
        else if (cmd == "set" && tokens.size() >= 2 && tokens[1] == "allocator") {
            // map 'set allocator first_fit' -> memory mode, cmd="set", arg="first_fit"
            mode = "mem";
            cmd = "set";
            ss = build_args(2);
        }
        else if (cmd == "dump" && tokens.size() >= 2) {
            string sub = tokens[1];
            if (sub == "memory") { mode = "mem"; cmd = "dump"; ss = build_args(2); }
            else if (sub == "buddy") { mode = "buddy"; cmd = "dump"; ss = build_args(2); }
        }
        else if (cmd == "stats" && tokens.size() >= 2) {
            string sub = tokens[1];
            if (sub == "memory") { mode = "mem"; cmd = "stats"; ss = build_args(2); }
            else if (sub == "cache") { mode = "cache"; cmd = "stats"; ss = build_args(2); }
            else if (sub == "vm") { mode = "vm"; cmd = "stats"; ss = build_args(2); }
        }

        // Keep backward compatibility: if no mode selected, require mode (unless the command set above selected one)
        if (mode.empty() && !(cmd == "init" && ss.str().size()>0 && (tokens.size()>=2))) {
            cout << "Select mode first\n";
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
                if (!ss && tokens.size() >= 2 && tokens[1] == "memory") {
                    // fallback if parsing failed but original tokens present
                    size = stoi(tokens.back());
                }
                delete memAlloc;
                memAlloc = new MemAllocator(size);
                cout << "Memory initialized\n";
            }
            else if (cmd == "set")
            {
                string a; ss >> a;
                if (!memAlloc) { cout << "Not initialized\n"; continue; }
                if (a == "first" || a == "first_fit") memAlloc->setallocator(FIRST_FIT);
                else if (a == "best" || a == "best_fit") memAlloc->setallocator(BEST_FIT);
                else if (a == "worst" || a == "worst_fit") memAlloc->setallocator(WORST_FIT);
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
