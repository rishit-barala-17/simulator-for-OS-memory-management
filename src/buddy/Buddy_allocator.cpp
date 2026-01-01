#include "buddy_alloc.h"
#include <iostream>
#include <map>
#include <set>
#include <iomanip>
using namespace std;



static int adjustBlockSize2(int size)
{
    int p = 1;
    while (p < size) p <<= 1;
    return p;
}



static int getBuddyAddress(int address, int blockSize)
{
    return address ^ blockSize;
}



BuddyAllocator::BuddyAllocator(int memorysize)
{
    total_memory = memorysize;
    freeLists.clear();
    freeLists[memorysize].insert(0);
}



int BuddyAllocator::memoryAlloc(int size)
{
    int reqSize = adjustBlockSize2(size);

    auto it = freeLists.lower_bound(reqSize);
    if (it == freeLists.end())
    {
        cout << "\n=== ALLOCATION FAILED ===\n";
        cout << "Requested Size : " << size << " bytes\n";
        cout << "Reason         : No suitable free block\n";
        return -1;
    }

    int currSize = it->first;
    int startAddr = *it->second.begin();
    it->second.erase(it->second.begin());
    
    if (it->second.empty())
    {
    	freeLists.erase(it);
    }    
    
    while (currSize > reqSize)
    {
        currSize = currSize / 2;
        freeLists[currSize].insert(startAddr + currSize);
    }

    cout << "\n=== ALLOCATION SUCCESS ===\n";
    cout << "Start Address   : 0x" << hex << startAddr << dec << "\n";
    cout << "Requested Size  : " << size << " bytes\n";
    cout << "Allocated Size  : " << reqSize << " bytes\n";

    return startAddr;
}



void BuddyAllocator::freeBlock(int address, int size)
{
    int blockSize = adjustBlockSize2(size);

    while (blockSize < total_memory)
    {
        int buddy = getBuddyAddress(address, blockSize);
        auto it = freeLists.find(blockSize);

        if (it == freeLists.end() || it->second.count(buddy) == 0)
            break;

        it->second.erase(buddy);
        if (it->second.empty())
            freeLists.erase(it);

        address = min(address, buddy);
        blockSize *= 2;
    }

    freeLists[blockSize].insert(address);

    cout << "\n=== FREE SUCCESS ===\n";
    cout << "Start Address  : 0x" << hex << address << dec << "\n";
    cout << "Block Size     : " << blockSize << " bytes\n";
    
}


void BuddyAllocator::memoryDump()
{
    ios oldState(nullptr);
    oldState.copyfmt(cout);

    cout << "\n== B U D D Y   F R E E   L I S T S ==\n";
    cout << left 
         << setw(20) << "    Size"
         << "Block Addresses\n";
    cout << "-------------------------------------\n";

    for (auto &entry : freeLists)
    {
        cout << left << "     " << setw(20)  << entry.first;

        if (entry.second.empty())
        {
            cout << "(none)";
        }
        else
        {
            for (int addr : entry.second)
            {
                cout << "0x" << hex << addr << " ";
            }
        }
        cout << dec << "\n";
    }

    cout << "-------------------------------------\n";

    cout.copyfmt(oldState);
}

