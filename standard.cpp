#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <map>
#include <cctype>
#include <chrono>
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
    map<string, int> data;
    int lineNum = 1;
    auto start_tp = chrono::steady_clock::now();
    while(getline(inFS, line)){
        bool startString = false;
        int startIndex;
        bool endString = false;
        int endIndex;
        bool startNum = false;
        int startNumIndex;
        int endNumIndex = 0;
        bool firstNum = false;
        for(unsigned int i = 0; i < line.size(); i++){
            if(line.at(i) == ' ' || line.at(i) == '\t'){
                continue;
            }
            if(!startString && line.at(i) == '"'){
                startString = true;
                startIndex = i;
                i++;
            }
            //string part
            while(startString && !endString){
                if(line.at(i) == '\\'){
                    if(line.at(i+1) == '\\' || line.at(i+1) == '"') i+=2;
                    else{
                        cout << "Error at line " << lineNum << "." << endl; //error
                        return 0;
                    }
                }
                if(line.at(i) == '"'){
                    endString = true;
                    endIndex = i+1;
                }
                else if(line.at(i) < 0x20 || line.at(i) > 0x7E){
                    cout << "Error at line " << lineNum << "." << endl;
                    return 0;
                }
                i++;
            }
            if(line.at(i) == '0'){
                if(endString && !startNum)continue;
            }
            if(endString && isdigit(line.at(i))){
                startNum = true; //allow zeros in the number
                if(!firstNum){
                    startNumIndex = i;
                    firstNum = true;
                }
            }
            if(startNum){
                if(line.at(i) == ' ' || line.at(i) == '\t'){
                    endNumIndex = i-1;
                    break;
                }
            }
        }
        if(endNumIndex == 0) endNumIndex = line.size();
        string key = line.substr(startIndex, endIndex-startIndex);
        int value = stoi(line.substr(startNumIndex, endNumIndex-1));
        lineNum++;
        auto it = data.find(key);
        if(it != data.end()){
            if(value > it->second){
                it->second = value;
            }
        }
        else{
            data[key] = value;
        }
    }
    auto stop_tp = chrono::steady_clock::now();
    auto duration = chrono::duration<double>(stop_tp - start_tp);
    cout << "Elapsed time: " << duration.count() << endl;
    inFS.close();
    string outputFile = string(argv[1]) + "-results";
    ofstream outFile(outputFile);
    for (const auto& pair : data) {
        outFile << pair.first << " " << pair.second << endl;
    }
}