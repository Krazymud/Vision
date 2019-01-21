#ifndef POINTMATCH_HPP
#include "CImg.h"
#include <map>
#include <set>
#include <iostream>
#include <cmath>
#include <vector>
extern "C" {
	#include "vl/generic.h"
	#include "vl/sift.h"
    #include "vl/kdtree.h"
}
using namespace std;
using namespace cimg_library;

#define NUM_OF_PAIR 4
#define RATE 0.99
#define INLIER_RATIO 0.5
#define MATCH_LIMIT 20
#define INLIER_LIMIT 4.0

struct matching_points{
    VlSiftKeypoint src;
    VlSiftKeypoint dst;
    matching_points(VlSiftKeypoint x, VlSiftKeypoint y){
        src = x;
        dst = y;
    }
};

struct Homoparam{
    float a, b, c, d, e, f, g, h;
    Homoparam(float a, float b, float c, float d, float e, float f, float g, float h){
        this->a = a;
        this->b = b;
        this->c = c;
        this->d = d;
        this->e = e;
        this->f = f;
        this->g = g;
        this->h = h;
    }
};

float xInSrc(float, float, Homoparam);
float yInSrc(float, float, Homoparam);

class Matching{
private:
    vector<vector<int>> matchIndex;
    vector<CImg<unsigned char>> srcImgs;
    vector<map<vector<float>, VlSiftKeypoint>> features;
    bool toStitch[MATCH_LIMIT][MATCH_LIMIT] = {false};
public:
    Matching(vector<CImg<unsigned char>> &, vector<map<vector<float>, VlSiftKeypoint>>&);
    vector<vector<int>>& getMatchIndex();
    void findAdjacent();
    bool isStitchNeed(int, int);
    int random(int, int);
    void removeStitch(int, int);
    vector<matching_points> findFromFeatures(map<vector<float>, VlSiftKeypoint> &, map<vector<float>, VlSiftKeypoint> &);
    Homoparam ransac(vector<matching_points>&);
    Homoparam getHomography(vector<matching_points>&);
    vector<int> getInliers(vector<matching_points>&, Homoparam, set<int> &);
    
    Homoparam leastSquare(vector<matching_points> &, vector<int> &);
};

#endif