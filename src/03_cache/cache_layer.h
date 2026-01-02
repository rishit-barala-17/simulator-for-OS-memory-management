#ifndef CACHE_LAYER_H
#define CACHE_LAYER_H

#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <list>
using namespace std;


enum class ReplacementPolicy {
    FIFO,
    LRU,
    LFU
};


class CacheLayer {
private:
    int maxEntries;
    ReplacementPolicy policy;

    unordered_set<int> table;

    queue<int> fifoOrder;

    list<int> lruOrder;
    unordered_map<int, list<int>::iterator> lruPos;

    unordered_map<int, int> freq;

    int hitCount;
    int missCount;

    void evict();

public:
    CacheLayer(int size, ReplacementPolicy p);

    bool probe(int addr);
    void push(int addr);

    int getHits() const;
    int getMisses() const;
    double hitRatio() const;

    void printStats(const char* label) const;
};


class CacheHierarchy {
private:
    CacheLayer l1;
    CacheLayer l2;
    CacheLayer l3;
    int totalPenalty;

public:
    CacheHierarchy(int l1Size, int l2Size, int l3Size, ReplacementPolicy p);
    void read(int addr);
    void printStats() const;
};


#endif
