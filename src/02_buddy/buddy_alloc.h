#ifndef BUDD_ALLOC_H
#define BUDD_ALLOC_H

#include <map>
#include <set>
using namespace std;


class BuddyAllocator
{
    private:
        int total_memory;
        map<int, set<int>> freeLists;
        
    public:
        BuddyAllocator(int memorysize);

        int memoryAlloc(int size);
        void freeBlock(int address, int size);

        void memoryDump();
};


#endif
