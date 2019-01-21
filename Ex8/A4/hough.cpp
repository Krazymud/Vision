#include "hough.h"

Hough::Hough(const char *filepath){
    img = CImg<unsigned char>(filepath);
    if(img.width() > img.height()){
        img = rotate();
    }
    duplicate = img;
    double vote_thres = 0.3;
    //Preprocessing
    cout << "Canny start...";
    canny can(img);
    res = can.result();
    cout << "Done" << endl << endl;
    //process the img
    cout << "Hough start..." << endl;
    InitHoughSpace();
    VoteForLines(vote_thres);
    CalLines();
    DrawIntersects();
    DrawEdges();
    cout << "Done" << endl << endl;
}

CImg<unsigned char> Hough::getImg(){
    return duplicate;
}

vector<pair<int, int>> Hough::getPoints(){
    return intersects;
}

CImg<unsigned char> Hough::rotate(){
    CImg<unsigned char> res(img.height(), img.width(), 1, 3, 0);
    for(int i = 0; i < img.width(); ++i){
        for(int j = img.height() - 1; j >= 0; --j){
            res(img.height() - 1 - j, i, 0) = img(i, j, 0);
            res(img.height() - 1 - j, i, 1) = img(i, j, 1);
            res(img.height() - 1 - j, i, 2) = img(i, j, 2);
        }
    }
    return res;
}

void Hough::InitHoughSpace(){
    int maxR = (int)sqrt(res.width() * res.width() + res.height() * res.height());
    hough_space = CImg<int>(360, maxR, 1, 1, 0);
    cimg_forXY(res, x, y){
        if(res(x, y) == 0){
            continue;
        }
        cimg_forX(hough_space, s){
            double theta = (double)s * cimg::PI / 180;
            int r = (int)(x * cos(theta)) + (int)(y * sin(theta));
            if(r > 0 && r < maxR){
                hough_space(s, r)++;
                maxH = maxH > hough_space(s, r) ? maxH : hough_space(s, r);
            }
        }
    }
}

void Hough::VoteForLines(double vote_thres){
    vector<pair<int, int>> rdn_points;  //redundant points
    cimg_forXY(hough_space, theta, r){
        if(theta == 0) continue;
        if(hough_space(theta, r) > maxH * vote_thres){
            bool flag = true;
            for(int i = 0; i < rdn_points.size(); ++i){
                if(sqrt((rdn_points[i].first - theta) * (rdn_points[i].first - theta) + 
                (rdn_points[i].second - r) * (rdn_points[i].second - r)) < 50){
                    flag = false;
                    if(hough_space(rdn_points[i].first, rdn_points[i].second) < hough_space(theta, r)){
                        rdn_points[i].first = theta;
                        rdn_points[i].second = r;
                    }
                }
            }
            if(flag){
                rdn_points.push_back(make_pair(theta, r));
            }
        }
    }
    for(int n = 0; n < 4; ++n){
        int xpos = 0, ypos = 0, max = 0;
        for(int i = 0; i < rdn_points.size(); ++i){
            if(max < hough_space(rdn_points[i].first, rdn_points[i].second)){
                max = hough_space(rdn_points[i].first, rdn_points[i].second);
                xpos = rdn_points[i].first;
                ypos = rdn_points[i].second;
            }
        }
        points.push_back(make_pair(xpos, ypos));
        hough_space(xpos, ypos) = 0;
    }
}
void Hough::CalLines(){
    for(auto p : points){
        double theta = double(p.first) * cimg::PI / 180.0;
        double k = -cos(theta) / sin(theta);
        double b = double(p.second) / sin(theta);
        lines.push_back(make_pair(k, b));
        cout << "Line: " << "y = " << k << "x + " << b << endl;
    }
}

void Hough::DrawIntersects(){
    for(int i = 0; i < lines.size() - 1; ++i){
        for(int j =  i + 1; j < lines.size(); ++j){
            double 
                k0 = lines[i].first,
                k1 = lines[j].first,
                b0 = lines[i].second,
                b1 = lines[j].second,
                x = (b1 - b0) / (k0 - k1),
                y = k0 * x + b0;
            if(x >= 0 && x < img.width() && y >= 0 && y < img.height()){
                intersects.push_back(make_pair(x, y));
            }
        }
    }
    for (int i = 0; i < intersects.size(); i++) {
        img.draw_circle(intersects[i].first, intersects[i].second, 50, red);
    }
}

void Hough::DrawEdges(){
    for(int i = 0; i < lines.size(); ++i){
        vector<pair<int, int>> temp;
        for(int j = 0; j < lines.size(); ++j){
            if(j == i) continue;
            double 
                k0 = lines[i].first,
                k1 = lines[j].first,
                b0 = lines[i].second,
                b1 = lines[j].second,
                x = (b1 - b0) / (k0 - k1),
                y = k0 * x + b0;
            if(x >= 0 && x < img.width() && y >= 0 && y < img.height()){
                temp.push_back(make_pair(x, y));
            }
        }
        img.draw_line(temp[0].first, temp[0].second, temp[1].first, temp[1].second, blue);
    }
}

