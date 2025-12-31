#ifndef ALLOCATOR_H
#define ALLOCATOR_H
#include <vector>
using namespace std;

	struct Block {
	    int start;	
	    int size;			
	    int requested; 		
	    bool isFree;
	    int id;
	};

enum Algorithm_type{
    FIRST_FIT,
    BEST_FIT,
    WORST_FIT
};

class MemAllocator {
    private:
        vector<Block> blocks;
        int total_memory;
        int next_id;
        int total_requests;
        int successful_allocs;
        
        int chooseBlock(int size, Algorithm_type type);
        void coalesceFreeBlocks();
        
    public:
        MemAllocator(int memorysize);
        
        void setallocator(Algorithm_type type);
        void memoryAlloc(int size); 
        void freeBlock(int id);
        
        void memoryDump();
        void memoryInfo();
        
        Algorithm_type which_algo; 
};

#endif