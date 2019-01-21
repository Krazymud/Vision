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
    int scale, sigma, lThres, hThres;
    cout << "Enter mask scale for gaussian filtering: ";
    cin >> scale;
    cout << "Enter sigma for gaussian filtering: ";
    cin >> sigma;
    cout << "Enter low threshold: ";
    cin >> lThres;
    cout << "Enter high threshold: ";
    cin >> hThres;
    vector<vector<double>> filter = createFilter(1 + scale * 2, 1 + scale * 2, sigma);

    //Print filter
    for (int i = 0; i<filter.size(); i++) 
    {
        for (int j = 0; j<filter[i].size(); j++) 
        {
            cout << filter[i][j] << " ";
        }
    }
    img.display("original");
    toGrayScale();  //Grayscale the image
    grayscaled.display("GrayScaled");
    useFilter(grayscaled, filter);  //Gaussian Filter
    gFiltered.display("Gaussian Blur");
    sobel();    //Sobel Filter
    sFiltered.display("Sobel Filtered");
    nonMaxSupp();   //Non-Maxima Suppression
    non.display("Non-Maxima Supp.");
    threshold(lThres, hThres);  //Double Threshold and Finalize
    thres.display("Double threshold");
    connect();  //connect edges
    conn.display("Connect edges");
    removeEdge();   //remove edges shorter than 20
    imgleft.display("Remove edges");
}
void canny::toGrayScale(){
    CImg<unsigned char> gray(img.width(), img.height(), 1, 1, 0);
    cimg_forXY(img, x, y){
        gray(x, y) = (img(x, y, 0) * 19595 + img(x, y, 1) * 38469 + img(x, y, 2) * 7472) >> 16;
    }
    grayscaled = gray;
}

vector<vector<double>> canny::createFilter(int row, int column, double sigmaIn){
	vector<vector<double>> filter;

	for (int i = 0; i < row; i++){
        vector<double> col;
        for (int j = 0; j < column; j++){
            col.push_back(-1);
        }
		filter.push_back(col);
	}

	float coordSum = 0;
	float constant = 2.0 * sigmaIn * sigmaIn;

	// Sum is for normalization
	float sum = 0.0;

	for (int x = - row/2; x <= row/2; x++){
		for (int y = -column/2; y <= column/2; y++){
			coordSum = (x*x + y*y);
			filter[x + row/2][y + column/2] = (exp(-(coordSum) / constant)) / (M_PI * constant);
			sum += filter[x + row/2][y + column/2];
		}
	}

	// Normalize the Filter
	for (int i = 0; i < row; i++){
        for (int j = 0; j < column; j++){
            filter[i][j] /= sum;
        }
    }
	return filter;

}

void canny::useFilter(CImg<unsigned char> img_in, vector<vector<double>> filterIn){
    int size = (int)filterIn.size() / 2;
    CImg<unsigned char> gaussianFI(img_in.width(), img_in.height(), 1, 1, 0);
	for (int i = 0; i < img_in.width(); i++){
		for (int j = 0; j < img_in.height(); j++){
			double sum = 0;
            
			for (int x = 0; x < filterIn.size(); x++){
				for (int y = 0; y < filterIn.size(); y++){
                    int xadd = 0, yadd = 0;
                    xadd = (j + x - size >= 0 && j + x - size < img_in.height()) ? x - size : size - x;
                    yadd = (i + y - size >= 0 && i + y - size < img_in.width()) ? y - size : size - y;
                    sum += filterIn[x][y] * (double)(img_in(i + yadd, j + xadd));
				}
            }
            gaussianFI(i, j) = sum;
		}

	}
	gFiltered = gaussianFI;
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
    non = CImg<unsigned char>(sFiltered.width(), sFiltered.height(), 1, 1, 0);
    for (int i = 0; i < sFiltered.width(); i++) {
        for (int j = 0; j < sFiltered.height(); j++) {
            non(i, j) = sFiltered(i, j);
        }
    }
    for (int i = 1; i < sFiltered.width() - 1; i++) {
        for (int j = 1; j < sFiltered.height() - 1; j++) {
            float Tangent = angles(i, j);

            // Vertical Edge
            if (((-22.5 < Tangent) && (Tangent <= 22.5)) || ((157.5 < Tangent) && (Tangent <= -157.5)))
            {
                if ((sFiltered(i, j) < sFiltered(i+1, j)) || (sFiltered(i, j) < sFiltered(i-1, j)))
                    non(i, j) = 0;
            }
            // Horizontal Edge
            if (((-112.5 < Tangent) && (Tangent <= -67.5)) || ((67.5 < Tangent) && (Tangent <= 112.5)))
            {
                if ((sFiltered(i, j) < sFiltered(i, j-1)) || (sFiltered(i, j) < sFiltered(i, j+1)))
                    non(i, j) = 0;
            }
            
            // 45 Degree Edge
            if (((-67.5 < Tangent) && (Tangent <= -22.5)) || ((112.5 < Tangent) && (Tangent <= 157.5)))
            {
                if ((sFiltered(i, j) < sFiltered(i - 1, j - 1)) || (sFiltered(i,j) < sFiltered(i + 1, j + 1)))
                    non(i, j) = 0;
            }
            
            // -45 Degree Edge
            if (((-157.5 < Tangent) && (Tangent <= -112.5)) || ((22.5 < Tangent) && (Tangent <= 67.5)))
            {
                if ((sFiltered(i,j) < sFiltered(i + 1, j - 1)) || (sFiltered(i,j) < sFiltered(i - 1, j + 1)))
                    non(i, j) = 0;
            }
        }
    }
}

void canny::threshold(int low, int high){
    if(low > 255)
        low = 255;
    if(high > 255)
        high = 255;
    thres = CImg<unsigned char>(non.width(), non.height(), 1, 1, 0);
    for(int i = 0; i < non.width(); i++){
        for(int j = 0; j < non.height(); j++){
            thres(i, j) = non(i, j);
            if(thres(i, j) > high){
                thres(i, j) = 255;
            }
            else if(thres(i, j) < low){
                thres(i, j) = 0;
            }
            else{
                bool anyHigh = false;
                bool anyBetween = false;
                for(int x = i - 1; x < i + 2; x++){
                    for(int y = j - 1; y < j + 2; y++){
                        if(x < 0 || y < 0 || x >= thres.width() || y >= thres.height()){
                            continue;
                        }
                        else{
                            if(non(x, y) > high){
                                thres(i,j) = 255;
                                anyHigh = true;
                                break;
                            }
                            else if(non(x,y) <= high && non(x,y) >= low)
                                anyBetween = true;
                        }
                    }
                    if(anyHigh)
                        break;
                }
                if(!anyHigh && anyBetween){
                    for (int x=i-2; x < i+3; x++) {
                        for (int y = j-2; y<j+3; y++) 
                        {
                            if(x < 0 || y < 0 || x >= thres.width() || y >= thres.height()){    //Out of bounds
                                continue;
                            }
                            else{
                                if(non(x,y) > high){
                                    thres(i,j) = 255;
                                    anyHigh = true;
                                    break;
                                }
                            }
                        }
                        if(anyHigh)
                            break;
                    }
                }
                if(!anyHigh){
                    thres(i,j) = 0;
                }
            }
        }
    }
}

void canny::connect(){
    conn = CImg<unsigned char>(thres.width(), thres.height(), 1, 1, 0);
    for(int i = 0; i < thres.width(); i++){
        for(int j = 0; j < thres.height(); j++){
            conn(i, j) = thres(i, j);
            if(conn(i, j) == 0){
                vector<pair<int, int>> store;   //存储八邻阈有无两条边
                for(int x = i - 1; x <= i + 1; x++){
                    for(int y = j - 1; y <= j + 1; y++){
                        if(x < 0 || y < 0 || x >= thres.width() || y >= thres.height()){
                            continue;
                        }
                        if(thres(x, y) > 0){
                            store.push_back(make_pair(x, y));
                        }
                    }
                }
                if(store.size() < 2){
                    continue;
                }
                bool flag = true;
                for(int a = 0; a < store.size() - 1; a++){
                    for(int b = a + 1; b < store.size(); b++){
                        if(abs(store[a].first - store[b].first) + abs(store[a].second - store[b].second) == 1){
                            flag = false;
                        }
                    }
                }
                if(flag){
                    conn(i, j) = 255;
                }
            }
        }
    }
}

void canny::removeEdge(){
    imgleft = CImg<unsigned char>(conn.width(), conn.height(), 1, 1, 0);
    for(int i = 0; i < conn.width(); i++){
        for(int j = 0; j < conn.height(); j++){
            imgleft(i, j) = conn(i, j);
        }
    }
    CImg<int> visited(conn.width(), conn.height(), 1, 1, 0);
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
            if(path.size() < 20){
                for(int x = 0; x < path.size(); x++){
                    imgleft(path[x].first, path[x].second) = 0;
                }
            }
        }
    }
}
