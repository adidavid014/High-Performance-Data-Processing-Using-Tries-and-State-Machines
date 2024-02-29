#include <iostream>
#include <fstream>
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
        val = -1;
        end = false;
    }    
};

Node trie[NUM_NODES];
int nextIndex = 1;

void insert(char* key, size_t len, int val){
    int currIndex = 0; //root

    for(size_t i = 0; i < len; i++){
        int charIndex = key[i] - ' '; //ASCII range is 0x20 to 0x7E
        if(charIndex < 0 || charIndex > 95){
            //error
            continue;
        }

        if(trie[currIndex].children[charIndex] == -1){ //no child inserted in this index yet
            trie[currIndex].children[charIndex] = nextIndex;
            trie[nextIndex] = Node();
            nextIndex++;
        }

        currIndex = trie[currIndex].children[charIndex];
    }
    trie[currIndex].end = true;

    if(val > trie[currIndex].val){
        trie[currIndex].val = val;
    }
}

void dfs(const Node& node, string str, ofstream& outFile){
    if(node.end){
        outFile << "\"" << str << "\" " << node.val << endl;
    }
    for(int i = 0; i < NUM_CHARS; i++){
        if(node.children[i] != -1){
            char nextChar = ' ' + i;
            dfs(trie[node.children[i]], str+nextChar, outFile);
        }
    }
}

enum State{
    READ_WHITESPACE,
    READ_STRING,
    READ_INT
};

int lineNum = 1;

int parseData(const char* data, size_t fileSize){
    char charString[20];
    int charIndex = 0;
    string currString = "";
    int num = 0;
    bool startInt = false;
    State currState = READ_WHITESPACE;
    for(size_t i = 0; i < fileSize; i++){
        switch(currState){
            case READ_WHITESPACE:
                if(data[i] == ' ' || data[i] == '\t' || data[i] == '\n'){
                    continue;
                }
                else if(data[i] == '"'){ //whitespace before string
                    currState = READ_STRING;
                }
                else if(isdigit(data[i])){ //whitespace between string and int
                    currState = READ_INT;
                    i--;
                }
                else{
                    cout << "Error at line " << lineNum << "." << endl;
                    return 0;
                }
                break;
            case READ_STRING:
                if(data[i] == '\\'){
                    if(data[i+1] == '\\' || data[i+1] == '"'){
                        charString[charIndex] = data[i];
                        charString[charIndex+1] = data[i+1];
                        charIndex+=2;
                        i++;
                    }
                    else{
                        cout << "Error at line " << lineNum << "." << endl;
                        return 0;
                    }
                }
                else if(data[i] == '"'){
                    currState = READ_WHITESPACE;
                }
                else{
                    charString[charIndex] = data[i];
                    charIndex++;
                    currString += data[i];
                }
                break;
            case READ_INT:
                if(isdigit(data[i])){
                    if(data[i] == '0' && startInt == false){
                        continue;
                    }
                    else if(data[i] != '0' && startInt == false){
                        num = num*10 + (data[i] - '0');
                        startInt = true;
                    }
                    else{
                        num = num * 10 + (data[i] - '0');
                    }
                }
                else if(data[i] == '\n' || data[i] == ' ' || data[i] == '\t'){
                    insert(charString, charIndex, num);
                    lineNum++;
                    currString = "";
                    charIndex = 0;
                    memset(charString, '\0', 20);
                    num = 0;
                    startInt = false;
                    currState = READ_WHITESPACE;
                }
                else{
                    cout << "Error at line " << lineNum << "." << endl;
                    return 0;
                }
                if(i == (fileSize-1)){
                    insert(charString, charIndex, num);
                    currState = READ_WHITESPACE;
                }
                break;
        }
    }
    return 1;
}


int main(int argc, char *argv[]){
    //open the file
    if(argc != 2){
        //error
        cout << "argc error" << endl;
        return 1;
    }
    const char* fileName = argv[1];
    int fd = open(fileName, O_RDONLY);
    if(fd == -1){
        //error
        cout << "fd error" << endl;
        return 1;
    }

    struct stat fileInfo;
    if(fstat(fd, &fileInfo) == -1){
        cout << "fstat error" << endl;
        //error
        return 1;
    }

    char* data = static_cast<char*>(mmap(nullptr, fileInfo.st_size, PROT_READ, MAP_PRIVATE, fd, 0));
    if(data == MAP_FAILED){
        //error
        cout << "map error" << endl;
        close(fd);
        return 1;
    }



    if(parseData(data, fileInfo.st_size) == 0){
        return 0;
    }

    string outputFile = string(argv[1]) + "-results";
    ofstream outFile(outputFile);

    dfs(trie[0], "", outFile);

    if(munmap(data, fileInfo.st_size) == -1){
        //error
    }
    close(fd);
    return 0;
}

