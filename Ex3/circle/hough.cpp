#include "hough.h"

Hough::Hough(const char *filepath){
    img = CImg<unsigned char>(filepath);
    double vote_thres = 0.5;
    int minr, maxr;
    cout << "Enter r range(min and max): " << endl;
    cin >> minr >> maxr;
    //Preprocessing
    canny can(filepath);
    res = can.result();
    res.display();
    VoteForR(minr, maxr);
    VoteForCircles();
    DrawCircles();
    cout << "Number of coins: " << circle.size() << endl;
    img.display();
}

void Hough::VoteForR(int minr, int maxr){
    int xp, yp, rp;
    for(int r = minr; r < maxr; r += 1){
        hough_space = CImg<int>(res.width(), res.height(), 1, 1, 0);
        cimg_forXY(res, x, y){
            if(res(x, y) == 0) continue;
            for(int theta = 1; theta < 360; theta += 4){
                int a = x - r * cos((double)theta * cimg::PI / 180);
                int b = y - r * sin((double)theta * cimg::PI / 180);
                if(a >= 0 && a < res.width() && b >= 0 && b < res.height()){
                    ++hough_space(a, b);
                    maxH = hough_space(a, b) > maxH ? hough_space(a, b) : maxH;
                }
            }
        }
        
        int max = 0;    //for this r
        cimg_forXY(hough_space, x, y){
            if(hough_space(x, y) > max){
                max = hough_space(x, y);
            }
        }
        vote.push_back(make_pair(max, r));
    }
    sort(vote.begin(), vote.end(), [](const pair<int, int>& x, const pair<int, int>& y) -> int {
        return x.first > y.first;
    });
}

void Hough::VoteForCircles(){
    for(auto v : vote){
        if(v.first > maxH * 0.5){
            hough_space = CImg<int>(res.width(), res.height(), 1, 1, 0);
            int xp, yp, max = 0;
            cimg_forXY(res, x, y){
                if(res(x, y) == 0) continue;
                for(int theta = 1; theta < 360; theta += 4){
                    int a = x - v.second * cos((double)theta * cimg::PI / 180);
                    int b = y - v.second * sin((double)theta * cimg::PI / 180);
                    if(a >= 0 && a < res.width() && b >= 0 && b < res.height()){
                        ++hough_space(a, b);
                        if(hough_space(a, b) > max){
                            max = hough_space(a, b);
                            xp = a;
                            yp = b;
                        }
                    }
                }
            }
            circle.push_back(make_pair(make_pair(xp, yp), v.second));
        }
        else break;
    }
}

void Hough::DrawCircles(){
    for(int i = 0; i < circle.size() - 1; i++){
        vector<int> todelete;
        for(int j = i + 1; j < circle.size(); j++){
            if(sqrt((circle[j].first.first - circle[i].first.first) * (circle[j].first.first - circle[i].first.first)
            + (circle[j].first.second - circle[i].first.second) * (circle[j].first.second - circle[i].first.second)) < 65){
                todelete.push_back(j);
            }
        }
        for(int j = todelete.size() - 1; j >= 0; j--){
            circle.erase(circle.begin() + todelete[j]);
        }
    }
    for(auto c : circle){
        img.draw_circle(c.first.first, c.first.second, 4, red);
        for(int r = c.second - 3; r < c.second + 3; ++r){
            img.draw_circle(c.first.first, c.first.second, r, red, 1.0, 1);
        }
    }
}

