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
    while(getline(inFS, line)){
        
    }

}