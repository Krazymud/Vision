#include "morph.h"
#include <iostream>
using namespace std;
using namespace cimg_library;
int main(){
    int a, p, b, mode;
    char file1[20], file2[20];
    cout << "Enter image path: ";
    scanf("%s", file1);
    scanf("%s", file2);
    cout << "Enter parameters a, p, b: ";
    cin >> a >> p >> b;
    cout << "Enter mode (0 for gray, 1 for RGB): ";
    cin >> mode;
    Morph m(file1, file2, a, p, b, mode);
    return 0;
}