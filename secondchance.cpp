#include <iostream>
#include <algorithm>
#include <utility>
#include "secondchance.h"

SecondChance::SecondChance(int size)
    : size(size) {}

SecondChance::~SecondChance() {}

void SecondChance::insert(int pagenum, bool exist)
{
    // not present in page table
    if (!exist) {
        // victim page is the first page inserted
        if (list.empty()) {
            victim = pagenum;
        }

        // pages inserts in the back of the list
        list.push_back(pagenum);

        // initialize rbit (aka mapped value) to true
        map.insert(map.begin(), { pagenum, true });
    }
    // mark that the page deserves a second chance
    else {
        auto it = map.find(pagenum);
        it->second = true;
    }
}

std::pair<int, int> SecondChance::replace(int pagenum, Hashtable* currentHT, Hashtable* otherHT, bool exist)
{
    std::pair <int,int> frameAndDirty;
    frameAndDirty = std::make_pair(-1, 0);

    // not present in page table
    if (!exist) {
        auto currentVictim = std::find(list.begin(), list.end(), victim);
        auto nextVictim = currentVictim;
        auto find = map.find(victim);

        while (true) {
            // rbit is false -> replace page
            if (find->second == false) {
                // insert new page
                list.insert(currentVictim, pagenum);
                map.insert(map.begin(), { pagenum, true });

                nextVictim = currentVictim;
                nextVictim++;

                // remove from page table
                frameAndDirty = currentHT->clear(find->first, otherHT);

                // remove from both list and map
                list.remove(find->first);
                map.erase(find->first);

                // when reached end of the list next victim's iterator points to the head
                if (nextVictim == list.end()) {
                    nextVictim = list.begin();
                }

                // update victim page
                find = map.find(*nextVictim);
                victim = find->first;

                break;
            }
            // mark rbit 'false' as it got one chance
            // and will be replaced next time unless accessed again
            else {
                find->second = false;
            }

            // move to the next page in list
            currentVictim++;

            // when reached end of the list current victim's iterator points to the head
            if (currentVictim == list.end()) {
                currentVictim = list.begin();
            }

            find = map.find(*currentVictim);
        }
    }
    // present in page table
    else {
        // mark that the page deserves a second chance
        auto it = map.find(pagenum);
        if (it != map.end()) {
            it->second = true; // rbit = 1
        }
    }

    return frameAndDirty;
}
