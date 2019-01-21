#ifndef HISTOEQUAL_HPP
#include "CImg.h"
#include <vector>
using namespace std;
using namespace cimg_library;

class Histo{
private:
    CImg<double> img;
    CImg<double> grayscaled;
    CImg<double> equalized;
    vector<double> count;
public:
    Histo(const char*, int);
    void ToGrayScaled();
    void EqualizeG();
    void EqualizeHSV();
};

#endif