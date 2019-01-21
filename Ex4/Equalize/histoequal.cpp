#include "histoequal.h"
#include <iostream>
using namespace std;
Histo::Histo(const char *filepath, int mode){
    img = CImg<double>(filepath);
    if(mode == 0){
        ToGrayScaled();
        equalized = CImg<double>(grayscaled);
        EqualizeG();
        CImg<double> row;
        row = grayscaled.append(equalized, 'x');
        row.display("gray - equalized");
        CImg<int> histImg1 = grayscaled.histogram(256, 0, 255);
        histImg1.display_graph("original", 3);
        CImg<int> histImg2 = equalized.histogram(256, 0, 255);
        histImg2.display_graph("equalized", 3);
    }
    else{
        equalized = CImg<double>(img);
        equalized.RGBtoHSV();
        CImg<double> originL(equalized.width(), equalized.height(), 1, 1, 0);
        cimg_forXY(equalized, x, y){originL(x, y) = equalized(x, y, 2) * 255;}
        EqualizeHSV();
        CImg<double> afterL(equalized.width(), equalized.height(), 1, 1, 0);
        cimg_forXY(equalized, x, y){afterL(x, y) = equalized(x, y, 2) * 255;}
        equalized.HSVtoRGB();
        CImg<double> row;
        row = img.append(equalized);
        row.display("rgb - equalized");
        CImg<int> histImg1 = originL.histogram(256, 0, 255);
        histImg1.display_graph("original", 3);
        CImg<int> histImg2 = afterL.histogram(256, 0, 255);
        histImg2.display_graph("equalized", 3);
    }
}

void Histo::ToGrayScaled(){
    grayscaled = CImg<double>(img.width(), img.height(), 1, 1, 0);
    cimg_forXY(img, x, y){
        grayscaled(x, y) = img(x, y, 0) * 0.299 + img(x, y, 1) * 0.587 + img(x, y, 2) * 0.114;
    }
}

void Histo::EqualizeHSV(){
    int m = equalized.width(), n = equalized.height();
    vector<double> t(256, 0);
    count = t;
    
    cimg_forXY(equalized, x, y){
        count[equalized(x, y, 2) * 255] += 1;
    }
    for(int i = 0; i < count.size(); i++){
        count[i] /= (m * n);
        if(i > 0){
            count[i] += count[i - 1];
        }
    }
    cimg_forXY(equalized, x, y){
        equalized(x, y, 2) = count[equalized(x, y, 2) * 255];
    }
}

void Histo::EqualizeG(){
    vector<double> t(256, 0);
    count = t;
    int m = img.width(), n = img.height();
    cimg_forXY(equalized, x, y){
        count[equalized(x, y)] += 1;
    }
    for(int i = 0; i < count.size(); i++){
        count[i] /= (m * n);
        if(i > 0){
            count[i] += count[i - 1];
        }
    }
    for(auto &c : count){
        c = 255 * c + 0.5;
    }
    cimg_forXY(equalized, x, y){
        equalized(x, y) = count[equalized(x, y)];
    }
}

