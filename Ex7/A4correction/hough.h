//
//  hough.h

#ifndef _HOUGH_
#define _HOUGH_
#include "CImg.h"
#include <vector>
#include <iostream>
using namespace std;
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
    Hough(CImg<unsigned char>, CImg<unsigned char>);
    void initHoughSpace();  //init hough space
    void voteForLines(double);  //voting
    void calLines();   //Calculate lines' param
    void drawIntersects();  //draw intersecting points
    void drawEdges();   //draw A4 edges
    vector<pair<int, int>> getIntersects(); //return intersects for warping
};

#endif