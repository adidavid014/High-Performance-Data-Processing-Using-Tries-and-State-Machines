#include <iostream>
#include <vector>
#include <cstring>

#define MAX_NODES 10000
#define NUM_CHARS 95

struct Node{
    int children[NUM_CHARS];
    int val;
    bool end;

    Node(){
        for(int i = 0; i < NUM_CHARS; i++){
            children[i] = -1;
        }
        val = 0;
        end = false;
    }    
};

