#ifndef CORRECT_HPP
#define CORRECT_HPP
#include "CImg.h"
#include <vector>
#include <sstream>
#include <algorithm>
using namespace std;
using namespace cimg_library;

struct Homoparam{
    double a, b, c, d, e, f, g, h;
    Homoparam(double a, double b, double c, double d, double e, double f, double g, double h){
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

class Correct{
private:
    CImg<unsigned char> img;
    CImg<unsigned char> res;
    vector<pair<int, int>> points;
    vector<pair<int, int>> dst;
    int width, height;
public:
    Correct(CImg<unsigned char>, vector<pair<int, int>>, int);
    void reorderPoints();
    void bilinear(int, int, double, double);
    void correcting(Homoparam &);
    Homoparam getHomography();
    CImg<unsigned char> getRes();
};


#endif