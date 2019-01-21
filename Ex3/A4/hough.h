//
//  hough.h

#ifndef _HOUGH_
#define _HOUGH_
#include "CImg.h"
#include "canny.h"
using namespace cimg_library;

class Hough{
private:
    const double red[3] = {255, 0, 0};
    const double blue[3] = {0, 0, 255};
    int maxH = 0;
    CImg<unsigned char> img; //Original Image
    CImg<unsigned char> res; //Preprocessed Image
    CImg<int> hough_space;    //hough space
    vector<pair<int, int>> points;  //points in parameter space
    vector<pair<int, int>> intersects;  //intersecting points
    vector<pair<double, double>> lines;  //lines
public:
    Hough(const char*);
    void InitHoughSpace();  //init hough space
    void VoteForLines(double);  //voting
    void CalLines();   //Calculate lines' param
    void DrawIntersects();  //draw intersecting points
    void DrawEdges();   //draw A4 edges
};

#endif