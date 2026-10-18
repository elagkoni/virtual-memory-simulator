#ifndef SECONDCHANCE_H
#define SECONDCHANCE_H
#include <list>
#include <unordered_map>
#include "hashtable.h"

class SecondChance
{
private:
    int size;
    std::list<int> list;                // contains page #
    std::unordered_map<int, bool> map;  // contains page # + rbit
    int victim;                         // victim page
public:
    SecondChance(int);
    ~SecondChance();

    void insert(int, bool);
    std::pair<int, int> replace(int, Hashtable*, Hashtable*, bool);
};

#endif // SECONDCHANCE_H
