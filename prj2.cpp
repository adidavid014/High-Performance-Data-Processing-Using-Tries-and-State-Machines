#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <map>
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

        }
    }

}