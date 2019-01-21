#include "histoequal.h"
#include "colorT.h"
#include <iostream>
#include <cstdio>
using namespace std;

int main(int argc, char *argv[]){
    int mode;
    cout << "Enter mode (0 for gray, 1 for RGB): ";
    cin >> mode;
    Histo h(argv[1], mode);
    
}