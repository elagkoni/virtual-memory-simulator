#include <iostream>
#include <cstring>
#include <cmath>
#include <fstream>
#include "hashtable.h"
#include "lru.h"
#include "secondchance.h"

#define PAGE_SIZE 4096
#define MIN_ARGUMENTS 4
#define ALL_REFERENCES 2000000
#define LGC_ADDRESS_SIZE 8      // 8 hex characters

#define SUCCESS 0
#define FAIL -1

using namespace std;

int main(int argc, char** argv)
{
    int algorithmType = 0, memoryFrames = 0, q = 0, maxReferences = 0;
    int totalWrites = 0, totalReads = 0, pf = 0, totalReferences = 0;

    if (argc < MIN_ARGUMENTS) {
        cout << "Not enough arguments" << endl;
        return FAIL;
    }

    //
    // get command line arguments
    //
    algorithmType = atoi(argv[1]);
    memoryFrames = atoi(argv[2]);
    q = atoi(argv[3]);

    // if max references not given in command line
    // read all references
    if (argc == MIN_ARGUMENTS) {
        maxReferences = ALL_REFERENCES;
    } else {
        maxReferences = atoi(argv[4]);
    }

    //
    // initialize main memory's frames to 0
    //
    unordered_map<int, int> mainMemory;         // contains frame # + 1/0 for occupied/empty
    for (int i = memoryFrames; i > 0; i--) {
        mainMemory.insert(pair<int, int>(i, 0));
    }

    //
    // open files
    //
    ifstream trace1("bzip.trace", ios::in), trace2("gcc.trace", ios::in);
    if (!trace1 || !trace2) {
        cout << "No such file";
        return FAIL;
    }
    ifstream* t = &trace1;

    //
    // variables for cropping offset's bits from logic address
    //
    const double offsetSize = log2(PAGE_SIZE);  // in bits (here 12)
    const int croppedBits = offsetSize / 4;     // how many hex characters we're going to crop
                                                // each hex character represents 4 bits thus we divide by 4

    string lgc(LGC_ADDRESS_SIZE, '\0');         // for logic address
    string rw(1, '\0');                         // for R/W operation
    int pageNum;
    std::pair <int,int> frameAndDirty;
    frameAndDirty = std::make_pair(-1, 0);

    Hashtable ht1(1, memoryFrames);
    Hashtable ht2(2, memoryFrames);
    Hashtable* currentPT = &ht1;
    Hashtable* otherPT = &ht2;

    LRU lru(memoryFrames);
    SecondChance sc(memoryFrames);

    // Steps:
    // 1) get logic address and R/W operation from trace
    // 2) crop last [croppedBits] bits from logic address
    // 3) convert hex characters of logic address to integer -> this is page number
    // 4) if page doesn't exist in current page table -> page fault occurs + reads counter increases
    // 5) iterate through main memory and check either for an empty frame or frame to replace
    // 6) if page doesn't exist in current page table -> insert
    //     else update page's dirty bit in current page table
    // 7) change input file and page table when you have read [q] traces from current file
    //
    while (totalReferences < maxReferences) {
        for (int i = 0; i < q; i++) {
            // (1)
            getline(*t, lgc, ' ');
            getline(*t, rw, '\n');
            // (2)
            lgc.erase(lgc.end() - croppedBits, lgc.end());
            // (3)
            pageNum = stoi(lgc, nullptr, 16);   // here is page reference
            // (4)
            bool exist = currentPT->search(pageNum);
            if (!exist) {
                pf++;
                totalReads++;   // reads increase whether we have Read or Write operation
            }

            // (5)
            for (auto it = mainMemory.begin(); it != mainMemory.end(); it++) {
                int full = std::distance(mainMemory.begin(), it) + 1;   // variable for checking number of frames that are full
                // found empty frame
                if (it->second == 0) {
                    if (!exist) {
                        it->second = 1;
                        frameAndDirty.first = it->first;    // get frame #
                    }

                    // insert page in replacement container
                    if (algorithmType == 1) {
                        lru.insert(pageNum, exist);
                    } else {
                        sc.insert(pageNum, exist);
                    }

                    break;
                }
                // memory is full -> find frame to replace
                else if (it->second != 0 && full == memoryFrames) {
                    // call replacement algorithm
                    if (algorithmType == 1) {
                        frameAndDirty = lru.replace(pageNum, currentPT, otherPT, exist);
                    } else {
                        frameAndDirty = sc.replace(pageNum, currentPT, otherPT, exist);
                    }

                    // if dirty bit=1 -> the page that we replaced is modified
                    if (frameAndDirty.second) {
                        totalWrites++;
                    }

                    break;
                }
            }

            // (6)
            if (!exist) {
                currentPT->insert(pageNum, frameAndDirty.first, &rw[0]);
            } else {
                currentPT->updateDirtyBit(pageNum, &rw[0]);
            }

            totalReferences++;

            // when maxReferences isn't multiple of q
            // stop loop for traces
            if (totalReferences == maxReferences) {
                break;
            }
        }

        // (7)
        if (memcmp((void*)t, (void*)&trace1, sizeof(trace1)) == 0) {
            t = &trace2;
            currentPT = &ht2;
            otherPT = &ht1;
        } else {
            t = &trace1;
            currentPT = &ht1;
            otherPT = &ht2;
        }
    }

    //
    // print statistics
    //
    if (algorithmType == 1) {
        cout << "Using LRU";
    } else {
        cout << "Using Second Chance";
    }
    cout << "\nTotal disk writes: " << totalWrites << "\nTotal disk reads: " << totalReads << "\nPage Faults: " << pf
         << "\nNumber of references (combined for both proccesses): " << totalReferences << "\nNumber of frames: " << memoryFrames << endl;

    return SUCCESS;
}
