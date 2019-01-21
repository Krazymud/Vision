#include "CImg.h"
#include <cmath>
#include <iostream>
using namespace cimg_library;
using namespace std;
const double PI = 3.14159265358979323846;
class Editimg{
    private:
        CImg<unsigned char> img;
    public:
        void loadimg(char *s){
            img.load_bmp(s);
        }
        void disp(){
            img.display();
        }
        void changecolor(){
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
        }
        void drawscircle(){
            cimg_forXY(img, x, y){
                if((x - 50) * (x - 50) + (y - 50) * (y - 50) <= 9){
                    img(x, y, 0) = 255;
                    img(x, y, 1) = 255;
                    img(x, y, 2) = 0;
                }
            }
        }
        void drawbcircle(){
            cimg_forXY(img, x, y){
                if((x - 50) * (x - 50) + (y - 50) * (y - 50) <= 900){
                    img(x, y, 0) = 0;
                    img(x, y, 1) = 0;
                    img(x, y, 2) = 255;
                }
            }
        }
        void drawline(){
            cimg_forXY(img, x, y){
                if(x*x + y*y <= 10000 && y - x * tan(35.0 * PI / 180.0) < 0.5 && y - x * tan(35.0 * PI / 180.0) > -0.5){
                    img(x, y, 0) = 0;
                    img(x, y, 1) = 0;
                    img(x, y, 2) = 255;
                }
            }
        }
        void save(char *s){
            img.save(s);
        }
};
int main() {
    Editimg img;
    char toload[] = "1.bmp";
    char tostore[] = "2.bmp";
    img.loadimg(toload);
    img.disp();
    img.changecolor();
    img.drawbcircle();
    img.drawscircle();
    img.drawline();
	img.disp();
    img.save(tostore);
	return 0;
}