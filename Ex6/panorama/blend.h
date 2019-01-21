#ifndef BLEND_HPP
#include "CImg.h"
#include <vector>
using namespace std;
using namespace cimg_library;
class Blend{
private:
    CImg<unsigned char> a;
    CImg<unsigned char> b;
    CImg<float> res;
public:
    Blend(CImg<unsigned char>&, CImg<unsigned char>&);
    void blending();
    bool isZero(CImg<unsigned char> &, int, int);
    CImg<unsigned char> returnRes();
};
#endif