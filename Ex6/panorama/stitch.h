#ifndef STITCH_HPP
#include "CImg.h"
//#include "warp.h"
#include "blend.h"
#include "feature.h"
#include "pointmatch.h"
#include "interpolation.h"
#include "warp.h"
#include <iostream>
#include <map>
#include <queue>
#include <vector>
using namespace std;
using namespace cimg_library;
int getMiddleIndex(vector<vector<int>> &);
void updateByHomo(map<vector<float>, VlSiftKeypoint> &, Homoparam, float, float);
void updateByOffset(map<vector<float>, VlSiftKeypoint> &feature, int, int);
void homoWarping(CImg<unsigned char> &, CImg<unsigned char> &, Homoparam, float, float);
void offsetMoving(CImg<unsigned char> &, CImg<unsigned char> &, float, float);
class Stitch{
    vector<CImg<unsigned char>> srcImgs;
    CImg<unsigned char> result;
public:
    CImg<unsigned char> toGrayImage(CImg<unsigned char> &);
    Stitch(vector<CImg<unsigned char>> &);
    void stitching();
    CImg<unsigned char> returnRes();
    vector<float> getSideLength(CImg<unsigned char> &, Homoparam, int, int);
};


#endif