#include "colorT.h"
#include <iostream>
using namespace std;
Trans::Trans(const char *img1, const char *img2){
    srcImg = CImg<double>(img1);
    targetImg = CImg<double>(img2);
    srcLab = RGBtoLab(srcImg);
    targetLab = RGBtoLab(targetImg);
    srcMeans = CompMeans(srcLab);
    targetMeans = CompMeans(targetLab);
    srcDev = CompDev(srcImg, srcMeans);
    targetDev = CompDev(targetImg, targetMeans);
    res = Correction();
    res = LabtoRGB(res);
    srcImg.append(targetImg);
    srcImg.display();
    res.display();
    srcImg.append(targetImg);
}

CImg<double> Trans::RGBtoLab(CImg<double> img){
    CImg<double> temp(img);
    CImg<double> result(img);
    cimg_forXY(img, x, y){
        double L = 0.3811 * img(x, y, 0) + 0.5783 * img(x, y, 1) + 0.0402 * img(x, y, 2);
        double M = 0.1967 * img(x, y, 0) + 0.7244 * img(x, y, 1) + 0.0782 * img(x, y, 2);
        double S = 0.0241 * img(x, y, 0) + 0.1228 * img(x, y, 1) + 0.8444 * img(x, y, 2);
        if (L == 0) L = 1;
        if (M == 0) M = 1;
        if (S == 0) S = 1;
        temp(x, y, 0) = log(L);
        temp(x, y, 1) = log(M);
        temp(x, y, 2) = log(S);
    }
    cimg_forXY(img, x, y){
        result(x, y, 0) = 1/sqrt(3) * temp(x, y, 0) + 1/sqrt(3) * temp(x, y, 1) + 1/sqrt(3) * temp(x, y, 2);
        result(x, y, 1) = 1/sqrt(6) * temp(x, y, 0) + 1/sqrt(6) * temp(x, y, 1) - 2/sqrt(6) * temp(x, y, 2);
        result(x, y, 2) = 1/sqrt(2) * temp(x, y, 0) - 1/sqrt(2) * temp(x, y, 1);
    }
    return result;
}

vector<double> Trans::CompMeans(CImg<double> img){
    double pixels = img.width() * img.height();
    vector<double> means(3, 0);
    double sum[3] = {0};
    cimg_forXY(img, x, y){
        sum[0] += img(x, y, 0);
        sum[1] += img(x, y, 1);
        sum[2] += img(x, y, 2);
    }
    means[0] = sum[0] /= pixels;
    means[1] = sum[1] /= pixels;
    means[2] = sum[2] /= pixels;
    return means;
}
vector<double> Trans::CompDev(CImg<double> img, vector<double> means){
    double pixels = img.width() * img.height();
    vector<double> var(3, 0);
    double sum[3] = {0};
    cimg_forXY(img, x, y){
        sum[0] += (img(x, y, 0) - means[0]) * (img(x, y, 0) - means[0]);
        sum[1] += (img(x, y, 1) - means[1]) * (img(x, y, 1) - means[1]);
        sum[2] += (img(x, y, 2) - means[2]) * (img(x, y, 2) - means[2]);
    }
    var[0] = sqrt(sum[0] /= pixels);
    var[1] = sqrt(sum[1] /= pixels);
    var[2] = sqrt(sum[2] /= pixels);
    return var;
}

CImg<double> Trans::LabtoRGB(CImg<double> img){
    CImg<double> temp(img);
    cimg_forXY(img, x, y){
        temp(x, y, 0) = sqrt(3)/3 * img(x, y, 0) + sqrt(6)/6 * img(x, y, 1) + sqrt(2)/2 * img(x, y, 2);
        temp(x, y, 1) = sqrt(3)/3 * img(x, y, 0) + sqrt(6)/6 * img(x, y, 1) - sqrt(2)/2 * img(x, y, 2);
        temp(x, y, 2) = sqrt(3)/3 * img(x, y, 0) - sqrt(6)/3 * img(x, y, 1);
    }
    cimg_forXY(img, x, y){
        img(x, y, 0) = pow(2.71828, temp(x, y, 0));
        img(x, y, 1) = pow(2.71828, temp(x, y, 1));
        img(x, y, 2) = pow(2.71828, temp(x, y, 2));
    }
    cimg_forXY(img, x, y){
        temp(x, y, 0) = 4.4679 * img(x, y, 0) - 3.5873 * img(x, y, 1) + 0.1193 * img(x, y, 2);
        temp(x, y, 1) = -1.2186 * img(x, y, 0) + 2.3809 * img(x, y, 1) - 0.1624 * img(x, y, 2);
        temp(x, y, 2) = 0.0497 * img(x, y, 0) - 0.2439 * img(x, y, 1) + 1.2045 * img(x, y, 2);
    }
    return temp;
}

CImg<double> Trans::Correction(){
    CImg<double> temp(srcLab);
    cimg_forXY(srcLab, x, y){
        srcLab(x, y, 0) -= srcMeans[0];
        srcLab(x, y, 1) -= srcMeans[1];
        srcLab(x, y, 2) -= srcMeans[2];
    }
    cimg_forXY(srcLab, x, y){
        temp(x, y, 0) = (targetDev[0] / srcDev[0]) * srcLab(x, y, 0) + targetMeans[0];
        temp(x, y, 1) = (targetDev[1] / srcDev[1]) * srcLab(x, y, 1) + targetMeans[1];
        temp(x, y, 2) = (targetDev[2] / srcDev[2]) * srcLab(x, y, 2) + targetMeans[2];
    }
    return temp;
}