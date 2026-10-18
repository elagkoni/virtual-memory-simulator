#ifndef LRU_H
#define LRU_H
#include <list>
#include "hashtable.h"

class LRU
{
private:
    int size;
    std::list<int> list;    // contains page #
public:
    LRU(int);
    ~LRU();

    void insert(int, bool);
    std::pair<int, int> replace(int, Hashtable*, Hashtable*, bool);
};

#endif // LRU_H
