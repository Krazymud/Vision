#include "correct.h"
#include <iostream>
Correct::Correct(CImg<unsigned char> img, vector<pair<int, int>> points, int i){
    this->img = img;
    this->points = points;
    cout << "A4 correcting start...";
    reorderPoints();
    
    res = CImg<unsigned char>(width, height, 1, 3, 0);
    Homoparam param = getHomography();
    correcting(param);
    string temp;
    stringstream s;
    s << i;
    s >> temp;
    res.save(("correct//" + temp + ".bmp").c_str());
    cout << "Done" << endl << endl;
}

CImg<unsigned char> Correct::getRes(){
    return res;
}

Homoparam Correct::getHomography(){
    CImg<double> A(4, 4, 1, 1, 0);
    CImg<double> b(1, 4, 1, 1, 0);
    //calc Ax = b
    for(int i = 0; i < 4; ++i){
        A(0, i) = dst[i].first;
        A(1, i) = dst[i].second;
        A(2, i) = dst[i].first * dst[i].second;
        A(3, i) = 1;
        b(0, i) = points[i].first;
    }
    CImg<double> x1 = b.get_solve(A);
    for(int i = 0; i < 4; ++i){
        b(0, i) = points[i].second;
    }
    CImg<double> x2 = b.get_solve(A);
    return Homoparam(x1(0,0), x1(0,1), x1(0,2), x1(0,3), x2(0,0), x2(0,1), x2(0,2), x2(0,3));
}

void Correct::reorderPoints(){
    sort(points.begin(), points.end(), [](pair<int, int> &a, pair<int, int> &b){
        return a.second < b.second;
    });
    double min = 100000;
    int temp = 0;
    for(int i = 1; i < points.size(); ++i){
        double dis = sqrt((points[0].first - points[1].first) * (points[0].first - points[1].first) + (points[0].second - points[1].second) * (points[0].second - points[1].second));
        if(dis < min){
            min = (dis < min) ? dis : min;
            temp = i;
        }
    }
    swap(points[1], points[temp]);
    if(points[0].first > points[1].first){
        swap(points[0], points[1]);
    }
    if(points[2].first < points[3].first){
        swap(points[2], points[3]);
    }
    width = sqrt((points[0].first - points[1].first) * (points[0].first - points[1].first) + (points[0].second - points[1].second) * (points[0].second - points[1].second));
    height = sqrt(2) * width;
    dst.push_back(make_pair(0, 0));
    dst.push_back(make_pair(width - 1, 0));
    dst.push_back(make_pair(width - 1, height - 1));
    dst.push_back(make_pair(0, height - 1));
}

void Correct::bilinear(int x, int y, double tx, double ty){
    int x_pos = floor(tx);
    double x_u = tx - x_pos;
    int xb = (x_pos < img.width() - 1) ? x_pos + 1 : x_pos;

    int y_pos = floor(ty);
	double y_v = ty - y_pos;
    int yb = (y_pos < img.height() - 1) ? y_pos + 1 : y_pos;

    for(int i = 0; i < 3; ++i){
        double P1 = img(x_pos, y_pos, i) * (1 - x_u) + img(xb, y_pos, i) * x_u;
        double P2 = img(x_pos, yb, i) * (1 - x_u) + img(xb, yb, i) * x_u;
        res(x, y, i) = P1 * (1 - y_v) + P2 * y_v;
    }
}

void Correct::correcting(Homoparam &param){
    cimg_forXY(res, x, y){
        double tx = (param.a * x + param.b * y + param.c * x * y + param.d),
        ty = (param.e * x + param.f * y + param.g * x * y + param.h);
        bilinear(x, y, tx, ty);
    }
}

