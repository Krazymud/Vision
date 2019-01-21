//
//  canny.cpp
//  Canny Edge Detector
//
//  Created by Hasan Akgün on 21/03/14.
//  Copyright (c) 2014 Hasan Akgün. All rights reserved.
//

#include <iostream>
#define _USE_MATH_DEFINES
#define M_PI 3.14159265358979323846
#include <cmath>
#include <vector>
#include <queue>
#include "canny.h"
using namespace cimg_library;
using namespace std;


canny::canny(CImg<unsigned char> file)
{
    img = file;

    toGrayScale();  //Grayscale the image
    gFiltered = grayscaled.get_blur(5.5);
    sobel();    //Sobel Filter
}

void canny::toGrayScale(){
    CImg<unsigned char> gray(img.width(), img.height(), 1, 1, 0);
    cimg_forXY(img, x, y){
        gray(x, y) = (img(x, y, 0) * 19595 + img(x, y, 1) * 38469 + img(x, y, 2) * 7472) >> 16;
    }
    grayscaled = gray;
}

void canny::sobel(){

    //Sobel X Filter
    double x1[] = {-1.0, 0, 1.0};
    double x2[] = {-2.0, 0, 2.0};
    double x3[] = {-1.0, 0, 1.0};

    vector<vector<double>> xFilter(3);
    xFilter[0].assign(x1, x1 + 3);
    xFilter[1].assign(x2, x2 + 3);
    xFilter[2].assign(x3, x3 + 3);
    
    //Sobel Y Filter
    double y1[] = {1.0, 2.0, 1.0};
    double y2[] = {0, 0, 0};
    double y3[] = {-1.0, -2.0, -1.0};
    
    vector<vector<double>> yFilter(3);
    yFilter[0].assign(y1, y1 + 3);
    yFilter[1].assign(y2, y2 + 3);
    yFilter[2].assign(y3, y3 + 3);
    
    //Limit Size
    int size = (int)xFilter.size() / 2;
    
	sFiltered = CImg<unsigned char>(gFiltered.width(), gFiltered.height(), 1, 1, 0);
    
	for (int i = 0; i < gFiltered.width(); i++){
		for (int j = 0; j < gFiltered.height(); j++){
			double sumx = 0;
            double sumy = 0;
            
			for (int x = 0; x < xFilter.size(); x++){
				for (int y = 0; y < xFilter.size(); y++){
                    int xadd = 0, yadd = 0;
                    xadd = (j + x - size >= 0 && j + x - size < gFiltered.height()) ? x - size : size - x;
                    yadd = (i + y - size >= 0 && i + y - size < gFiltered.width()) ? y - size : size - y;
                    sumx += xFilter[x][y] * (double)(gFiltered(i + yadd, j + xadd)); //Sobel_X Filter Value
                    sumy += yFilter[x][y] * (double)(gFiltered(i + yadd, j + xadd)); //Sobel_Y Filter Value
				}
            }
            double sumxsq = sumx * sumx;
            double sumysq = sumy * sumy;
            
            double sq2 = sqrt(sumxsq + sumysq);
            
            if(sq2 > 255) //Unsigned Char Fix
                sq2 = 255;
            sFiltered(i, j) = sq2;
		}
	}

    res = CImg<unsigned char>(sFiltered.width(), sFiltered.height(), 1, 1, 0);
    cimg_forXY(sFiltered, x, y){
        res(x, y) = sFiltered(x, y) > 30 ? 255 : 0;
    }
}


