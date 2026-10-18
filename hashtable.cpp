#include <iostream>
#include <utility>
#include <memory>
#include "hashtable.h"

#define EXIST 1
#define NOT_EXIST 0
#define FAIL -1

Hashtable::Hashtable(int id, int size)
    : tableId(id), size(size)
{
    table.assign(size, nullptr);
}

Hashtable::~Hashtable() {
    // delete all linked list nodes in each bucket
    for (size_t i = 0; i < table.size(); ++i) {
        node* current = table[i];
        while (current != nullptr) {
            node* temp = current;
            current = current->next;
            delete temp;
        }
        table[i] = nullptr;
    }
}

int Hashtable::hashFunction(int key)
{
    return key % size;
}

void Hashtable::insert(int pageNum, int frameNum, std::string operation)
{
    // get bucket number
    int hashedKey = hashFunction(pageNum);

    // create new node and initialize its members with function's arguments
    node* entry = new node;
    entry->pageNumber = pageNum;
    entry->frameNumber = frameNum;
    if (operation[0] == 'R') {
        entry->dirtyBit = 0;
    } else if (operation[0] == 'W') {
        entry->dirtyBit = 1;
    }
    entry->next = nullptr;

    node* current = nullptr, * prev = nullptr;

    // move to the end of current bucket's list
    // and insert new node there
    if (table.at(hashedKey) != nullptr) {
        current = table.at(hashedKey);

        while (current != nullptr) {
            prev = current;
            current = current->next;
        }

        // update 'next' pointer of previous last node
        prev->next = entry;
    }
    // current bucket is empty
    else {
        table.at(hashedKey) = entry;
    }

    // update map
    std::vector<node*>::iterator it = table.begin();
    std::advance(it, hashedKey);
    map[pageNum] = it;
}

//
// check if page is present in table
//
bool Hashtable::search(int pageNum)
{
    // present -> 1
    // not present -> 0

    if (map.find(pageNum) == map.end()) {
        return NOT_EXIST;
    }
    return EXIST;
}

//
// remove page from the table and return <frame #, dirtyBit> of the removed page
//
std::pair<int, int> Hashtable::clear(int pageNum, Hashtable* otherHT)
{
    std::pair <int,int> frameAndDirty;

    // requested page is not present in current table
    // find it and remove it from the other
    if (!search(pageNum)) {
        frameAndDirty = otherHT->clear(pageNum, this);
        return frameAndDirty;
    }

    node* current = nullptr, * prev = nullptr;
    auto find = map.find(pageNum);
    current = *find->second;

    // find requested page in table
    while (current != nullptr
           && current->pageNumber != pageNum) {
        prev = current;
        current = current->next;
    }
    frameAndDirty = std::make_pair(current->frameNumber, current->dirtyBit);

    // requested page to remove is the head of the list
    // bucket now points to next node or NULL
    if (current == *find->second) {
        *find->second = current->next;
    }
    // update previous node
    else {
        prev->next = current->next;
    }
    map.erase(find);

    return frameAndDirty;
}

//
// set page's dirty bit '1' if operation is 'W' (indicates that page is modified)
//
void Hashtable::updateDirtyBit(int pageNum, std::string operation)
{
    if (!search(pageNum)) {
        return;
    }

    node* current = nullptr;
    auto find = map.find(pageNum);  // find bucket
    current = *find->second;
    while (current != nullptr
           && current->pageNumber != pageNum) {
        current = current->next;
    }
    if (operation[0] == 'W') {
        current->dirtyBit = 1;
    }
}
