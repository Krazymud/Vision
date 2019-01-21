#ifndef MORPH_HPP
#include "CImg.h"
#include <vector>
#include <utility>
#include <iostream>
#include <fstream>
using namespace std;
using namespace cimg_library;


const int frameCount = 11;
struct Line{
    pair<int, int> P;
    pair<int, int> Q;
    double len;
    Line(){}
    Line(pair<int, int> p, pair<int, int> q){
        P = p;
        Q = q;
        len = sqrt((p.first - q.first) * (p.first - q.first) + (p.second - q.second) * (p.second - q.second));
    }
    pair<double, double> GetCorrPoint(double, double);
    double GetLen();
    double GetDist(double, double, pair<int, int>);
    double Getu(pair<int, int> x);
    double Getv(pair<int, int> x);
};

struct LinePair{
    Line leftLine;
    Line rightLine;
    vector<Line> warpLine;
    void GetWarpLine();
    LinePair(Line l, Line r){
        leftLine = l;
        rightLine = r;
        GetWarpLine();
    }
};

class Morph{
private:
    int frameIndex = 0, mode = 0;
    const int color[3] = {216, 216, 216};
    int a, p, b; //warping parameter
    CImg<unsigned char> srcImg;
    CImg<unsigned char> desImg;
    vector<pair<int, int>> srcPoints;
    vector<pair<int, int>> desPoints;
    vector<LinePair> linePairs;
public:
    Morph(const char *, const char *, int, int, int, int);
    void RecordPoints();
    void InitLines();
    void WarpAndMorph();
    CImg<unsigned char> Bilinear(double r, double c, CImg<unsigned char> des);
};

#endif