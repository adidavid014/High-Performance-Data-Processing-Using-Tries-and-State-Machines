#include <iostream>
#include <fstream>
#include <cstring>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <chrono>
#include <iomanip>

#define NUM_NODES 200000
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

// void dfs(const Node& node, string str, ofstream& outFile){
//     if(node.end){
//         outFile << "\"" << str << "\" " << node.val << endl;
//     }
//     for(int i = 0; i < NUM_CHARS; i++){
//         if(node.children[i] != -1){
//             char nextChar = ' ' + i;
//             dfs(trie[node.children[i]], str+nextChar, outFile);
//         }
//     }
// }

void dfs(const Node& node, char* str, int charIndex, ofstream& outFile){
    if(node.end){
        outFile << "\"" << str << "\" " << node.val << endl;
    }
    for(int i = 0; i < NUM_CHARS; i++){
        if(node.children[i] != -1 && charIndex < 20){
            str[charIndex] = ' ' + i; 
            str[charIndex + 1] = '\0'; 
            dfs(trie[node.children[i]], str, charIndex + 1, outFile);
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
                else if(data[i] < 0x20 || data[i] > 0x7E){
                    cout << "Error at line " << lineNum << "." << endl;
                    return 0;
                }
                else{
                    charString[charIndex] = data[i];
                    charIndex++;
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
                    if(num == 0){
                        cout << lineNum;
                    }
                    insert(charString, charIndex, num);
                    lineNum++;
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
                    if(num == 0){
                        continue;
                    }
                    else{
                        insert(charString, charIndex, num);
                        currState = READ_WHITESPACE;
                    }
                }
                break;
        }
    }
    return 1;
}

void resetTrie(){
    for(int i = 0; i < NUM_NODES; i++){
        trie[i] = Node();
    }
    nextIndex = 1;
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

    //first run
    auto start_tp = std::chrono::steady_clock::now();
    if(parseData(data, fileInfo.st_size) == 0){
        return 0;
    }
    auto stop_tp = chrono::steady_clock::now();
    auto duration = chrono::duration<double>(stop_tp - start_tp);
    lineNum--;
    double firstTime = lineNum/duration.count();

    //second run
    lineNum = 1;
    resetTrie();
    start_tp = std::chrono::steady_clock::now();
    parseData(data, fileInfo.st_size);
    stop_tp = chrono::steady_clock::now();
    duration = chrono::duration<double>(stop_tp - start_tp);
    lineNum--;
    double secondTime = lineNum/duration.count();

    //third run
    lineNum = 1;
    resetTrie();
    start_tp = std::chrono::steady_clock::now();
    parseData(data, fileInfo.st_size);
    stop_tp = chrono::steady_clock::now();
    duration = chrono::duration<double>(stop_tp - start_tp);
    lineNum--;
    double thirdTime = lineNum/duration.count();

    double maxTime = std::max(firstTime, std::max(secondTime, thirdTime));
    if(maxTime >= 1000000){
        cout << fixed << setprecision(0) << "Lines per second: " << maxTime << endl;
    }
    else{
        cout << "Lines per second: " << maxTime << endl;
    }

    string outputFile = string(argv[1]) + "-results";
    ofstream outFile(outputFile);
    char dfsString[21];
    dfs(trie[0], dfsString, 0, outFile);

    if(munmap(data, fileInfo.st_size) == -1){
        //error
    }
    close(fd);
    return 0;
}

