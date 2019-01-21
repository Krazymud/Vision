#include "region.h"
#include <iostream>
Region::Region(const char * filepath){
    src = CImg<unsigned char>(filepath);
    src.display();
    toGrayScale();
    gray.blur(10);
    gray.display();
    point seed(src.width() / 2, src.height() / 2);
    int thres = 50;
    cout << "Enter param for regionGrowing: ";
    cin >> thres;
    regionGrowth(seed, thres);
    extractEdge();
}

CImg<unsigned char> Region::getRes(){
    return result;
}

CImg<unsigned char> Region::getImg(){
    return src;
}

void Region::extractEdge(){
    result = CImg<unsigned char>(grown.width(), grown.height(), 1, 1, 0);
    cimg_forXY(grown, x, y){
        bool has_black_nei = false, has_white_nei = false;
        for (int i = -1; i < 2; ++i) {
            for (int j = -1; j < 2; ++j) {
                if(i == 0 && j == 0) continue;
                if(isInsideImage(x + i, y + j)){
                    if(grown(x + i, y + j) == 255){
                        has_white_nei = true;
                    }
                    if(grown(x + i, y + j) == 0){
                        has_black_nei = true;
                    }
                }
            }
        }
        if(grown(x, y) == 255 && has_black_nei && has_white_nei){
            result(x, y) = 255;
        }
    }
    result.save("fuck.bmp");
    result.display();
}

void Region::toGrayScale(){
    gray = CImg<unsigned char>(src.width(), src.height(), 1, 1, 0);
    cimg_forXY(src, x, y){
        gray(x, y) = src(x, y, 0) * 0.299 + src(x, y, 1) * 0.587 + src(x, y, 2) * 0.114;
    }
}

void Region::regionGrowth(point seed, int threshold){
	// 输出结果图像
    grown = CImg<unsigned char>(gray.width(), gray.height(), 1, 1, 0);
    int T = threshold;
    int x, y;
    queue<point> seeds;
    point currentSeed;
    seeds.push(seed);

    grown(seed.x, seed.y) = 255;

    while(!seeds.empty()) {
        currentSeed = seeds.front();
        seeds.pop();
        x = currentSeed.x;
        y = currentSeed.y;
		// 遍历八邻域
        for (int i = -1; i < 2; ++i) {
            for (int j = -1; j < 2; ++j) {
                if(isInsideImage(x + i, y + j)){
					// 判断是否已经生长过，即结果图像中对应点的灰度值是否为 0
                    if(grown(x + i, y + j) != 255){
						// 生长条件的判断
                        if(gray(x + i, y + j) >= T){
							// 新种子点压栈
                            seeds.push(point(x + i, y + j));
							// 将结果图像中对应坐标的灰度值设置为 0
                            grown(x + i, y + j) = 255;
                        }
                    }
                }
            }
        }
    }
	// 保存输出结果图像
    grown.display();
}

bool Region::isInsideImage(int x, int y){
    return (x >= 0 && x < src.width() && y >= 0 && y < src.height());
}