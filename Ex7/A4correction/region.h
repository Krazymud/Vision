#ifndef REGION_HPP
#define REGION_HPP
#include "CImg.h"
#include <queue>
using namespace std;
using namespace cimg_library;
struct point{
    int x, y;
    point(int x_, int y_){
        x = x_;
        y = y_;
    }
    point(){
        x = 0;
        y = 0;
    }
};
class Region{
private:
    CImg<unsigned char> src;
    CImg<unsigned char> gray;
    CImg<unsigned char> grown;
    CImg<unsigned char> result;
public:
    Region(const char *);
    CImg<unsigned char> getImg();
    void toGrayScale();
    void regionGrowth(point, int);
    bool isInsideImage(int, int);
    CImg<unsigned char> getRes();
    void extractEdge();
};


#endif