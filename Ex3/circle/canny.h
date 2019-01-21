//
//  canny.h
//  Canny Edge Detector
//
//  Created by Hasan Akgün on 21/03/14.
//  Copyright (c) 2014 Hasan Akgün. All rights reserved.
//

#ifndef _CANNY_
#define _CANNY_
#include "CImg.h"
#include <vector>
#include <iostream>
using namespace std;
using namespace cimg_library;

class canny {
private:
    CImg<unsigned char> img; //Original Image
    CImg<unsigned char> grayscaled; // Gradient
    CImg<unsigned char> gFiltered; // Gradient
    CImg<unsigned char> sFiltered; //Sobel Filtered
    CImg<unsigned char> non; // Non-maxima supp.
    CImg<float> angles; //Angle Map
    CImg<unsigned char> imgleft; // Non-maxima supp.
public:
    canny(const char*); //Constructor
	void toGrayScale();
    void nonMaxSupp(); //Non-maxima supp.
    void sobel(); //Sobel filtering
    CImg<unsigned char> result(){return imgleft;} //Non-maxima supp.
    void removeEdge(); //remove edges shorter than 20
};

#endif