#include "CImg.h"
#include <cmath>
#include <iostream>
using namespace cimg_library;
using namespace std;
const double PI = 3.14159265358979323846;
int main() {
	unsigned char blue[] = {0, 0, 255};
	unsigned char yellow[] = {255, 255, 0};

	CImg<unsigned char> img;
	img.load_bmp("1.bmp");
	img.display();
	int w = img._width;
	int h = img._height;

    cimg_forXY(img, x, y){
        if(img(x, y, 0) == 255 && img(x, y, 1) == 255 && img(x, y, 2) == 255){
            img(x, y, 0) = 255;
            img(x, y, 1) = 0;
            img(x, y, 2) = 0;
        }
        else if(img(x, y, 0) == 0 && img(x, y, 1) == 0 && img(x, y, 2) == 0){
            img(x, y, 0) = 0;
            img(x, y, 1) = 255;
            img(x, y, 2) = 0;
        }
    }
	img.draw_circle(50, 50, 30, blue);
	img.draw_circle(50, 50, 3, yellow);
	img.draw_line(0, 0, cos(35.0 * PI / 180) * 100, sin(35.0 * PI / 180) * 100, blue);
	img.display();
	img.save("2.bmp");
	return 0;
}