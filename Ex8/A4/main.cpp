#include "CImg.h"
#include "split.h"
#include "hough.h"
#include "correct.h"
#include <cstdio>
using namespace cimg_library;

// Main procedure
//----------------
int main(int argc,char **argv) {
    for(int i = 1; i < argc; ++i){
        Hough h(argv[i]);
        Correct cr(h.getImg(), h.getPoints(), i - 1);
        Split sp(cr.getRes(), i - 1);
    }
    return 0;
}