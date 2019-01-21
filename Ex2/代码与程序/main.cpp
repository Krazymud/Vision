#include <iostream>
#include "canny.h"

using namespace std;

int main(){
    const char *filePath = "test_Data/stpietro.bmp";    //filePath of the input image
    canny cny(filePath);
    return 0;
}
//stpietro