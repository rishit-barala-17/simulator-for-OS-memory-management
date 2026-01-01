#include "mem_allocator.h"
#include <iostream>
#include <climits>
#include <iomanip>
using namespace std;



MemAllocator::MemAllocator(int memorysize) {

    which_algo = FIRST_FIT;

    total_requests = 0;
    successful_allocs = 0;

    total_memory = memorysize;
    next_id = 1;

    blocks.push_back({0, total_memory, 0, true, -1});
}



void MemAllocator::setallocator(Algorithm_type type)
{
    which_algo = type;
}
	
	
	
void MemAllocator::memoryAlloc(int size) 
{
   total_requests++;
   int idx = chooseBlock(size, which_algo);
   if (idx == -1)
    {
        cout << "\n=== ALLOCATION FAILED ===\n";
        cout << "Requested Size : " << size << " bytes\n";
        cout << "Reason         : No suitable free block\n";
        return;
    }

   successful_allocs++;
   int startAddr = blocks[idx].start;
   int prevSize = blocks[idx].size;

   blocks[idx].size = size;
   blocks[idx].requested = size;
   blocks[idx].isFree = false;
   blocks[idx].id = next_id++;

   if (prevSize > size)
	{
      Block rem;
	   rem.start = startAddr + size;
	   rem.size = prevSize - size;
	   rem.requested = 0;
	   rem.isFree = true;
	   rem.id = -1;
	   blocks.insert(blocks.begin() + idx + 1, rem);
	   }
	cout << "\n=== ALLOCATION SUCCESS ===\n";
    cout << "Algorithm      : ";
    if (which_algo == FIRST_FIT)      cout << "FIRST_FIT\n";
    else if (which_algo == BEST_FIT)  cout << "BEST_FIT\n";
    else if (which_algo == WORST_FIT) cout << "WORST_FIT\n";

    cout << "Block ID       : " << blocks[idx].id << "\n";
    cout << "Start Address  : 0x" << hex << blocks[idx].start << dec << "\n";
    cout << "Block Size     : " << size << " bytes\n";

}



int MemAllocator::chooseBlock(int size, Algorithm_type which_algo)
{
    int index = -1;

    if(which_algo == BEST_FIT)
    {
        int best = INT_MAX;
        for(int i = 0; i < blocks.size(); ++i)
        {
            if(blocks[i].isFree && blocks[i].size >= size && blocks[i].size < best)
            {
                best = blocks[i].size;
                index = i;
            }
        }
    }

    if(which_algo == WORST_FIT)
    {
        int worst = -1;
        for(int i = 0; i < blocks.size(); ++i)
        {
            if(blocks[i].isFree && blocks[i].size >= size && blocks[i].size > worst)
            {
                worst = blocks[i].size;
                index = i;
            }
        }
    }

	if(which_algo == FIRST_FIT)
    {
        for(int i = 0; i < blocks.size(); ++i)
        {
            if(blocks[i].isFree && blocks[i].size >= size) 
            {
            	return i;
            }
        }
    }
    
    return index; 
}

	

void MemAllocator::freeBlock(int id)
{
    for (auto &blk : blocks)
    {
        if (!blk.isFree && blk.id == id)
        {
            blk.isFree = true;
            blk.id = -1;
            blk.requested = 0;
            coalesceFreeBlocks();

            cout << "\n=== FREE SUCCESS ===\n";
            cout << "Block ID   : " << id << "\n";
            cout << "Status     : OK\n";
            return;
        }
    }
    cout << "\n=== FREE FAILED ===\n";
    cout << "Block ID : " << id << "\n";
    cout << "Status   : Block not found\n";
}



void MemAllocator::coalesceFreeBlocks()
{
    for(int i = 0; i < blocks.size() - 1; ++i)
    {
        if(blocks[i].isFree && blocks[i + 1].isFree)
        {
            blocks[i].size += blocks[i + 1].size;
            blocks.erase(blocks.begin() + i + 1);
            --i;
        }
    }
}


	
void MemAllocator::memoryDump()
{
    cout << "\n============ M E M O R Y   M A P ============\n";
    cout << left
         << setw(16) << "Address Range"
         << setw(9)  << "Status"
         << setw(11) << "Block ID"
         << setw(18) << "Size"
         << "\n";
    cout << "---------------------------------------------\n";

    for (auto &blk : blocks)
    {
        cout << "0x" << hex << setw(4) << blk.start
             << "- 0x" << setw(4) << (blk.start + blk.size - 1)
             << dec << "   ";

        if (blk.isFree)
        {
            cout << setw(11)  << "FREE"
                 << setw(8) << "-"
                 << setw(15) << (to_string(blk.size) + " bytes")
                 << "\n";
        }
        else
        {
            cout << setw(11)  << "USED"
                 << setw(8) << blk.id
                 << setw(15) << (to_string(blk.size) + " bytes")
                 << "\n";
        }
    }

    cout << "---------------------------------------------\n";
}




void MemAllocator::memoryInfo()
{
	 cout << dec;
	 int used = 0, freemem = 0, internal_frag = 0, largest_free = 0;

	 for(auto &blk : blocks)
	 {
	     if(blk.isFree)
	     {
	         freemem += blk.size;
	         largest_free = max(largest_free, blk.size);
	     } 
	     else 
	     {
	         used += blk.size;
	         internal_frag += blk.size - blk.requested;
	     }
	 }

	double external_frag = (freemem > 0) ? 100.0 * (double)(freemem - largest_free) / freemem : 0.0;

	double utilization = 100.0 * used / total_memory;
    
	double success_rate = (total_requests > 0) ? 100.0 * successful_allocs / total_requests : 0.0;

	cout << "\n===============================================\n";
	cout << "M E M O R Y   S T A T I S T I C S   R E P O R T\n";
	cout << "===============================================\n";
	
	cout << left << setw(35) << "Total Memory"
	     << ": " << total_memory << " bytes\n";
	
	cout << left << setw(35) << "Used Memory"
	     << ": " << used << " bytes\n";
	
	cout << left << setw(35) << "Free Memory"
	     << ": " << freemem << " bytes\n";
	
    cout << left << setw(35) << "Total Requests"
         << ": " << total_requests << "\n";

    cout << left << setw(35) << "Successful Allocations"
         << ": " << successful_allocs << "\n";

    cout << left << setw(35) << "Failed Allocations"
         << ": " << (total_requests - successful_allocs) << "\n";

	cout << left << setw(35) << "Internal Fragmentation"
	     << ": " << internal_frag << " bytes\n";
	
	cout << left << setw(35) << "External Fragmentation"
	     << ": " << fixed << setprecision(2) << external_frag << " %\n";
	
	cout << left << setw(35) << "Memory Utilization"
	     << ": " << fixed << setprecision(2) << utilization << " %\n";
	
	cout << left << setw(35) << "Allocation Success Rate"
	     << ": " << fixed << setprecision(2) << success_rate << " %\n";
		
	cout << "===============================================\n";

}
	
	
	