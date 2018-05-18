#include <iostream>
#include <sstream>
using namespace std;

int main(void){
    stringstream ss1;
    ss1 << 3;
    string num = ss1.str();
    ss1 << 5;
    string num2 = ss1.str();
    cout << num2 << endl;
}
