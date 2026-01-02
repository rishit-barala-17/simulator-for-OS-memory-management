#include "cache_layer.h"
#include <iostream>
#include <iomanip>
#include <climits>
using namespace std;



CacheLayer::CacheLayer(int size, ReplacementPolicy p)
    : maxEntries(size), policy(p), hitCount(0), missCount(0) {}



bool CacheLayer::probe(int addr) 
{
    if (table.count(addr)) 
    {
        hitCount++;

        if (policy == ReplacementPolicy::LRU) 
        {
            lruOrder.erase(lruPos[addr]);
            lruOrder.push_front(addr);
            lruPos[addr] = lruOrder.begin();
        }
        else if (policy == ReplacementPolicy::LFU) 
        {
            freq[addr]++;
        }

        return true;
    }

    missCount++;
    return false;
}



void CacheLayer::evict() 
{
    int victim;

    if (policy == ReplacementPolicy::FIFO) 
    {
        victim = fifoOrder.front();
        fifoOrder.pop();
    }
    else if (policy == ReplacementPolicy::LRU) 
    {
        victim = lruOrder.back();
        lruOrder.pop_back();
        lruPos.erase(victim);
    }
    else 
    {
        int minFreq = INT_MAX;
        for (auto &p : freq) 
        {
            if (p.second < minFreq) 
            {
                minFreq = p.second;
            }
        }
        for (auto &p : freq) 
        {
            if (p.second == minFreq) 
            {
                victim = p.first;
                break;
            }
        }
        freq.erase(victim);
    }

    table.erase(victim);
}



void CacheLayer::push(int addr)
{
    if (table.count(addr)) return;

    if ((int)table.size() == maxEntries) 
    {
        evict();
    }

    table.insert(addr);

    if (policy == ReplacementPolicy::FIFO) 
    {
        fifoOrder.push(addr);
    }
    else if (policy == ReplacementPolicy::LRU) 
    {
        lruOrder.push_front(addr);
        lruPos[addr] = lruOrder.begin();
    }
    else 
    {
        freq[addr] = 1;
    }
}



int CacheLayer::getHits() const 
{
    return hitCount;
}



int CacheLayer::getMisses() const 
{
    return missCount;
}



double CacheLayer::hitRatio() const 
{
    int total = hitCount + missCount;
    return total == 0 ? 0.0 : (double)hitCount / total;
}



void CacheLayer::printStats(const char* label) const 
{
    cout << label << " Cache Stats:\n";
    cout << "Hits: " << hitCount << endl;
    cout << "Misses: " << missCount << endl;
    cout << "Hit Ratio: " << fixed << setprecision(4) << hitRatio() << endl;
}



CacheHierarchy::CacheHierarchy(int l1Size, int l2Size, int l3Size, ReplacementPolicy p)
    : l1(l1Size, p), l2(l2Size, p), l3(l3Size, p), totalPenalty(0) {}



void CacheHierarchy::read(int addr) 
{
    if (l1.probe(addr)) 
    {
        cout << "L1 hit" << endl;
    }
    else if (l2.probe(addr)) 
    {
        cout << "L1 miss\nL2 hit" << endl;
        totalPenalty += 1;
        l1.push(addr);
    }
    else if (l3.probe(addr)) 
    {
        cout << "L1 miss\nL2 miss\nL3 hit" << endl;
        totalPenalty += 2;
        l2.push(addr);
        l1.push(addr);
    }
    else 
    {
        cout << "Cache miss at all levels -> data fetched from main memory" << endl;
        totalPenalty += 3;
        l3.push(addr);
        l2.push(addr);
        l1.push(addr);
    }
}



void CacheHierarchy::printStats() const 
{
    l1.printStats("L1");
    l2.printStats("L2");
    l3.printStats("L3");
    cout << "Total Miss Penalty (levels crossed): " << totalPenalty << endl;
}
