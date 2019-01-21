#include "CImg.h"
#include "hough.h"
#include <cstdio>
using namespace cimg_library;

// Main procedure
//----------------
int main(int argc,char **argv) {
    Hough hough(argv[1]);
    return 0;
}