//
//  hough.h

#ifndef _HOUGH_
#define _HOUGH_
#include "CImg.h"
#include "canny.h"
#include <algorithm>
using namespace cimg_library;

class Hough{
private:
    const double red[3] = {255, 0, 0};
    int maxH = 0;
    CImg<unsigned char> img; //Original Image
    CImg<unsigned char> res; //Preprocessed Image
    vector<pair<int, int>> vote; //vote for circles
    CImg<int> hough_space;    //hough space
    vector<pair<pair<int, int>, int>> circle; 
public:
    Hough(const char*);
    void VoteForR(int, int);  //init hough space
    void VoteForCircles();
    void DrawCircles();
};

#endif