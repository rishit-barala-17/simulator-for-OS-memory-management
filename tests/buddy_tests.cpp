#include "C:/Users/asus/Documents/Rishit IITR Academics/ACM Project/src/buddy/buddy_alloc.h"
#include <iostream>
using namespace std;

int main()
{
    BuddyAllocator buddy_ctrl(64);
    buddy_ctrl.memoryDump();

    int a = buddy_ctrl.memoryAlloc(1);
    buddy_ctrl.memoryDump();

    int b = buddy_ctrl.memoryAlloc(8);
    buddy_ctrl.memoryDump();

    int c = buddy_ctrl.memoryAlloc(9);
    buddy_ctrl.memoryDump();

    buddy_ctrl.freeBlock(b, 8);
    buddy_ctrl.memoryDump();

    buddy_ctrl.freeBlock(a, 1);
    buddy_ctrl.memoryDump();

    int d = buddy_ctrl.memoryAlloc(16);
    buddy_ctrl.memoryDump();

    buddy_ctrl.memoryAlloc(128);

    buddy_ctrl.freeBlock(c, 9);
    buddy_ctrl.freeBlock(d, 16);
    buddy_ctrl.memoryDump();

    return 0;
}
