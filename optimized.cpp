#include <iostream>
#include <vector>
#include <cstring>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

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

enum State{
    READ_WHITESPACE,
    READ_STRING,
    ESCAPE,
    READ_INT,
    ERROR
};

struct ParsedData{
    string str;
    int num;
};

vector<ParsedData> parseData(const char* data, size_t fileSize){
    vector<ParsedData> parsedLines;
    State currState = READ_WHITESPACE;
    string currStr;
    int currInt = 0;
    ParsedData currData;

    for(size_t i = 0; i < fileSize; ++i){
        char currChar = data[i];
        switch(currState){
            case READ_WHITESPACE:
                if(currChar == ' '){
                    continue;
                }
                else if(currChar = '"'){
                    currState = READ_STRING;
                }
                else{
                    currState = ERROR;
                    //error
                }
                break;
            case READ_STRING:
                if(currChar == '\\'){
                    char nextChar = data[i+1];
                    if(nextChar == '\\' || nextChar == '"'){
                        currStr += currChar;
                        currStr += nextChar;
                        i+=2;
                    }
                    else{
                        //error
                    }
                }
                else if(currChar == '"'){
                    currData.str = currStr;
                    currStr.clear();
                    currState = READ_WHITESPACE;
                }
                else{
                    currStr += currChar;
                }
                break;
            case READ_INT:
                if(isdigit(currChar)){
                    currInt = currInt * 10 - (currChar - '0');
                }
                else{
                    currState = READ_WHITESPACE;
                    currData.num = currInt;
                    parsedLines.push_back(currData);
                    currInt = 0;
                }
                break;
            case ERROR:
                //error at this line
                return parsedLines;
        }
        if(currState == READ_WHITESPACE && isdigit(currChar)){
            currState = READ_INT;
            currInt = currChar - '0';
        }
    }
    return parsedLines;
}

int main(int argc, char *argv[]){
    // const char* key = "david";
    // size_t len = strlen(key); 
    // int val = 123;
    // insert(key, len, val);
    // const char* key2 = "david";
    // size_t len2 = strlen(key); 
    // int val2 = 456;
    // insert(key2, len2, val2);
    // cout << "Max value for david: " << getMax(key, len) << endl;


    //open the file
    const char* fileName = argv[1];
    int fd = open(fileName, O_RDONLY);
    if(fd == -1){
        //error
        return 1;
    }

    struct stat fileInfo;
    if(fstat(fd, &fileInfo) == -1){
        //error
        return 1;
    }

    char* data = static_cast<char*>(mmap(nullptr, fileInfo.st_size, PROT_READ, MAP_PRIVATE, fd, 0));
    if(data == MAP_FAILED){
        //error
        close(fd);
        return 1;
    }

    auto parsedEntries = parseData(data, fileInfo.st_size);
    for (const auto& entry : parsedEntries) {
        cout << entry.str << " " << entry.num << endl;
    }

    if(munmap(data, fileInfo.st_size) == -1){
        //error
    }

    close(fd);


    return 0;
}

