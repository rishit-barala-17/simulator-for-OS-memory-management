#include "C:/Users/asus/Documents/Rishit IITR Academics/ACM Project/src/01_mem/mem_allocator.h"

int main() {

    MemAllocator mem_ctrl(1024);

    mem_ctrl.setallocator(WORST_FIT);

    mem_ctrl.memoryAlloc(120);
    mem_ctrl.memoryDump();

    mem_ctrl.memoryAlloc(180);
    mem_ctrl.memoryDump();

    mem_ctrl.memoryAlloc(200);
    mem_ctrl.memoryDump();

    mem_ctrl.freeBlock(2);
    mem_ctrl.memoryDump();

    mem_ctrl.memoryAlloc(150);
    mem_ctrl.memoryDump();
    
    mem_ctrl.freeBlock(4);
    mem_ctrl.memoryDump();
    
    mem_ctrl.setallocator(BEST_FIT);
    
    mem_ctrl.memoryAlloc(140);
    mem_ctrl.memoryDump();
    
    mem_ctrl.memoryInfo();

    return 0;
}
