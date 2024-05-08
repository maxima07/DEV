#include <algorithm>
#include <iostream>
#include <iterator>
#include <string>

using namespace std;

void PrintSpacesPositions(std::string& str) {
    for(auto i = find(str.begin(), str.end(), ' '); 
        i != str.end(); 
        i = find(next(i), str.end(), ' ')){
        cout << distance(str.begin(), i) << endl;
    }
}

int main() {
    std::string str = "He said: one and one and one is three";
    PrintSpacesPositions(str);
    return 0;
}