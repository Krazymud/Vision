#ifndef COLORT_HPP
#include "CImg.h"
#include <cmath>
#include <vector>

using namespace std;
using namespace cimg_library;

class Trans{
private:
    CImg<double> srcImg;
    CImg<double> targetImg;
    CImg<double> srcLab;
    CImg<double> targetLab;
    CImg<double> res;
    vector<double> srcMeans;
    vector<double> targetMeans;
    vector<double> srcDev;
    vector<double> targetDev;
public:
    Trans(const char *, const char *);
    CImg<double> RGBtoLab(CImg<double>);
    CImg<double> LabtoRGB(CImg<double>);
    vector<double> CompMeans(CImg<double>);
    vector<double> CompDev(CImg<double>, vector<double>);
    CImg<double> Correction();

};

#endif