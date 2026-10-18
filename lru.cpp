#include <iostream>
#include <algorithm>
#include <utility>
#include "lru.h"

LRU::LRU(int size)
    : size(size) {}

LRU::~LRU() {}

void LRU::insert(int pagenum, bool exist)
{
    // present in page table
    if (exist) {
        list.erase(std::find(list.begin(), list.end(), pagenum));
    }

    // pages insert at the head of the list
    list.push_front(pagenum);
}

std::pair<int, int> LRU::replace(int pagenum, Hashtable* currentHT, Hashtable* otherHT, bool exist)
{
    std::pair <int, int> frameAndDirty;
    frameAndDirty = std::make_pair(-1, 0);

    // not present in page table
    if (!exist) {
        // delete least recently used element
        int victim = list.back();

        // pop the last element from list
        list.pop_back();

        // remove from page table
        frameAndDirty = currentHT->clear(victim, otherHT);
    }
    // present in memory
    else {
        list.erase(std::find(list.begin(), list.end(), pagenum));
    }

    // update reference
    list.push_front(pagenum);

    return frameAndDirty;
}
