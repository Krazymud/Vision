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
#include "CImg.h"
#include "canny.h"
using namespace cimg_library;
using namespace std;


canny::canny(const char *filename)
{
	CImg<unsigned char> image(filename);
    img = image;

    toGrayScale();  //Grayscale the image
    gFiltered = grayscaled.get_blur(3.8);
    sobel();    //Sobel Filter
    nonMaxSupp();
    removeEdge();
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
    
    angles = CImg<float>(gFiltered.width(), gFiltered.height(), 1, 1, 0); //AngleMap

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
 
            if(sumx==0) //Arctan Fix
                angles(i, j) = 90;
            else
                angles(i, j) = atan(sumy / sumx) * (180.0 / M_PI);
		}
	}
}

void canny::nonMaxSupp(){
    non = CImg<unsigned char>(sFiltered.width() - 1, sFiltered.height() - 1, 1, 1, 0);
    for (int i = 1; i < sFiltered.width() - 1; i++) {
        for (int j = 1; j < sFiltered.height() - 1; j++) {
            float Tangent = angles(i, j);
            non(i - 1, j - 1) = sFiltered(i, j);
            // Vertical Edge
            if (((-22.5 < Tangent) && (Tangent <= 22.5)) || ((157.5 < Tangent) && (Tangent <= -157.5)))
            {
                if ((sFiltered(i, j) < sFiltered(i+1, j)) || (sFiltered(i, j) < sFiltered(i-1, j)))
                    non(i - 1, j - 1) = 0;
            }
            // Horizontal Edge
            if (((-112.5 < Tangent) && (Tangent <= -67.5)) || ((67.5 < Tangent) && (Tangent <= 112.5)))
            {
                if ((sFiltered(i, j) < sFiltered(i, j-1)) || (sFiltered(i, j) < sFiltered(i, j+1)))
                    non(i - 1, j - 1) = 0;
            }
            
            // 45 Degree Edge
            if (((-67.5 < Tangent) && (Tangent <= -22.5)) || ((112.5 < Tangent) && (Tangent <= 157.5)))
            {
                if ((sFiltered(i, j) < sFiltered(i - 1, j - 1)) || (sFiltered(i,j) < sFiltered(i + 1, j + 1)))
                    non(i - 1, j - 1) = 0;
            }
            
            // -45 Degree Edge
            if (((-157.5 < Tangent) && (Tangent <= -112.5)) || ((22.5 < Tangent) && (Tangent <= 67.5)))
            {
                if ((sFiltered(i,j) < sFiltered(i + 1, j - 1)) || (sFiltered(i,j) < sFiltered(i - 1, j + 1)))
                    non(i - 1, j - 1) = 0;
            }
            non(i - 1, j - 1) = non(i - 1, j - 1) > 42 ? 255 : 0;
        }
    }
}

void canny::removeEdge(){
    imgleft = CImg<unsigned char>(non.width(), non.height(), 1, 1, 0);
    for(int i = 0; i < non.width(); i++){
        for(int j = 0; j < non.height(); j++){
            imgleft(i, j) = non(i, j);
        }
    }
    CImg<int> visited(non.width(), non.height(), 1, 1, 0);
    queue<pair<int, int> > mqueue;
    for(int i = 0; i < imgleft.width(); i++){
        for(int j = 0; j < imgleft.height(); j++){
            if(visited(i, j)){
                continue;
            }
            mqueue.push(make_pair(i, j));
            visited(i, j) = 1;
            vector<pair<int, int>> path;
            while(!mqueue.empty()){
                int head_i = mqueue.front().first, head_j = mqueue.front().second;
                path.push_back(make_pair(head_i, head_j));
                mqueue.pop();
                for(int x = head_i - 1; x <= head_i + 1; x++){
                    for(int y = head_j - 1; y <= head_j + 1; y++){
                        if(x < 0 || y < 0 || x >= imgleft.width() || y >= imgleft.height()){
                            continue;
                        }
                        else{
                            if(imgleft(x, y) > 0 && !visited(x, y)){
                                mqueue.push(make_pair(x, y));
                                visited(x, y) = 1;
                            }
                        }
                    }
                }
            }
            if(path.size() < 50){
                for(int x = 0; x < path.size(); x++){
                    imgleft(path[x].first, path[x].second) = 0;
                }
            }
        }
    }
}


