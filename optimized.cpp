#include <iostream>
#include <vector>
#include <cstring>
#include <sys/mman.h>


#define NUM_NODES 10000
#define NUM_CHARS 95

using namespace std;

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

vector<Node> trie(NUM_NODES);
int nextIndex = 1;

void insert(const char* key, size_t len, int val){
    int currIndex = 0; //root

    for(size_t i = 0; i < len; i++){
        int charIndex = key[i] - ' '; //ASCII range is 0x20 to 0x7E

        if(charIndex < 0 || charIndex > 95){
            //error
            continue;
        }

        if(trie[currIndex].children[charIndex] == -1){ //no child inserted in this index yet
            trie[currIndex].children[charIndex] = trie.size();
            trie.push_back(Node());
        }

        currIndex = trie[currIndex].children[charIndex];
    }
    trie[currIndex].end = true;

    if(val > trie[currIndex].val){
        trie[currIndex].val = val;
    }
}

int getMax(const char* key, size_t len){
    int nodeIndex = 0;

    for(size_t i = 0; i < len; i++){
        int charIndex = key[i] - ' ';
        if(trie[nodeIndex].children[charIndex] == -1){
            //error: string not found
            return -1;
        }
        nodeIndex = trie[nodeIndex].children[charIndex];
    }

    if(trie[nodeIndex].end){
        return trie[nodeIndex].val;
    }
    return -1;
}

int main(){
    const char* key = "david";
    size_t len = strlen(key); 
    int val = 123;
    insert(key, len, val);
    const char* key2 = "david";
    size_t len2 = strlen(key); 
    int val2 = 456;
    insert(key2, len2, val2);
    cout << "Max value for david: " << getMax(key, len) << endl;
    return 0;
}

