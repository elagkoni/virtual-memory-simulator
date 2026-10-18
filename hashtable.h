#ifndef HASHTABLE_H
#define HASHTABLE_H
#include <vector>
#include <map>
#include <unordered_map>

typedef struct node
{
    int pageNumber;
    int frameNumber;
    int dirtyBit;
    struct node* next;
} node;

class Hashtable
{
private:
    int tableId;
    int size;
    std::vector<node*> table;   // contains pointers to nodes
    std::unordered_map<int, std::vector<node*>::iterator> map; // contains node # + iterator to bucket
public:
    Hashtable(int, int);
    ~Hashtable();

    int hashFunction(int);

    void insert(int, int, std::string);
    bool search(int);
    std::pair<int, int> clear(int, Hashtable*);

    void updateDirtyBit(int, std::string);
};

#endif // HASHTABLE_H
