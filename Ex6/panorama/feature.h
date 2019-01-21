#ifndef FEATURE_HPP
#include "CImg.h"
#include <map>
#include <vector>
extern "C" {
	#include "vl/generic.h"
	#include "vl/sift.h"
}
using namespace std;
using namespace cimg_library;
#define RESIZE 500.0

class Feature{
private:
    CImg<unsigned char> gray;
    map<vector<float>, VlSiftKeypoint> features;
public:
    Feature(CImg<unsigned char> &);
    void extractFeatures();
    map<vector<float>, VlSiftKeypoint>& returnFeatures();
};

#endif