#ifndef VIRTUAL_MEMORY_H
#define VIRTUAL_MEMORY_H

#include <vector>
#include <deque>
using namespace std;


struct PageEntry 
{
    bool present;
    int frame_index;
};


class VirtualMemory 
{
    private:
        int pg_size;
        int total_pages;
        int total_frames;

        vector<PageEntry> table;
        deque<int> replace_queue;
        vector<char> frame_status;

        int fault_count;
        int access_count;

    public:
        VirtualMemory(int pageCount, int frameCount, int pageSize);

        int resolve(int vaddr, bool& fault);
        void touch(int vaddr);
        void printStats() const;
};


#endif
