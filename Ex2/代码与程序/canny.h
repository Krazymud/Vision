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
    CImg<unsigned char> grayscaled; // Grayscale
    CImg<unsigned char> gFiltered; // Gradient
    CImg<unsigned char> sFiltered; //Sobel Filtered
    CImg<float> angles; //Angle Map
    CImg<unsigned char> non; // Non-maxima supp.
    CImg<unsigned char> thres; //Double threshold and final
    CImg<unsigned char> conn;
    CImg<unsigned char> imgleft;  //connected edges
public:

    canny(const char*); //Constructor
	void toGrayScale();
	vector<vector<double>> createFilter(int, int, double); //Creates a gaussian filter
	void useFilter(CImg<unsigned char>, vector<vector<double>>); //Use some filter
    void sobel(); //Sobel filtering
    void nonMaxSupp(); //Non-maxima supp.
    void threshold(int, int); //Double threshold and finalize picture
    void removeEdge(); //remove edges shorter than 20
    void connect(); //connect edges
};

#endif