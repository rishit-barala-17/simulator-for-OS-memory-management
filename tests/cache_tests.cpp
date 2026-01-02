#include "C:/Users/asus/Documents/Rishit IITR Academics/ACM Project/src/03_cache/cache_layer.h"
#include <iostream>
using namespace std;

void runTest(ReplacementPolicy policy, const char* name) 
{
    cout << "\n==============================\n";
    cout << "Testing Policy: " << name << endl;
    cout << "==============================\n";

    CacheHierarchy cache(2, 4, 8, policy);

    int accesses[] = {1,2,3,4,5,6,7,8,7,3,1,100};
    int n = sizeof(accesses) / sizeof(accesses[0]);

    for (int i = 0; i < n; i++) 
    {
        cache.read(accesses[i]);
    }

    cache.printStats();
}

int main() 
{
    runTest(ReplacementPolicy::FIFO, "FIFO");
    runTest(ReplacementPolicy::LRU,  "LRU");
    runTest(ReplacementPolicy::LFU,  "LFU");

    return 0;
}
