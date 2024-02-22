#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <map>
#include <cctype>
using namespace std;

int main(int argc, char *argv[]){
    ifstream file(argv[1]);
    ifstream inFS;
    if(argc != 2){
        cout << "Invalid command line arguments" << endl;
        return 1;
    }
    string fileName = argv[1];
    inFS.open(fileName);
    if(!(inFS.is_open())){
        cout << "Could not open the file." << endl;
        return 1;
    }
    string line;
    bool startString = false;
    int startIndex;
    bool endString = false;
    int endIndex;
    bool startNum = false;
    int startNumIndex;
    int endNumIndex;

    map<string, int> data;
    while(getline(inFS, line)){
        for(int i = 0; i < line.size(); i++){
            if(line.at(i) == ' ' || line.at(i) == '\t') continue;
            if(!startString && line.at(i) == '"'){
                startString = true;
                startIndex = i;
            }
            //string part
            while(startString && !endString){
                if(line.at(i) == '\\'){
                    if(line.at(i+1) == '\\' || line.at(i+1) == '"') i+=2;
                    else cout << "error" << endl; //error
                }
                if(line.at(i == '"')){
                    endString = true;
                    endIndex = i;
                }
                i++;
            }
            while(endString && line.at(i) == '0' && !startNum) continue;
            if(endString && isdigit(line.at(i))){
                startNum = true; //allow zeros in the number
                startNumIndex = i;
            }
            if(startNum){
                if(line.at(i) == ' ' || line.at(i) == '\t'){
                    endNumIndex = i;
                    break;
                }
            }
        }
        string key = line.substr(startIndex, endIndex);
        int value = stoi(line.substr(startNumIndex, endNumIndex));
        auto it = data.find(key);
        if(it != data.end()) {
            if (value > it->second){
                it->second = value;
            }
        }
        else{
            data[key] = value;
        }
    }

}