#include <iostream>
#include <string>
#include "CImg.h"
#include "readimg.h"
#include "stitch.h"
using namespace std;
using namespace cimg_library;

int main(){
	string folder;
	vector<string> filenames;
	cout << "Enter image_dir's name: ";
	cin >> folder;
	getFileName(folder.c_str(), filenames);

	vector<CImg<unsigned char>> srcImgs(filenames.size());
	for(int i = 0; i < filenames.size(); ++i){
		CImg<unsigned char> img((folder + "\\" + filenames[i]).c_str());
		int width = img.width(), height = img.height();
    	float resize = (width < height) ? (RESIZE / width) : (RESIZE / height);
    	if(resize >= 1){
        	resize = 1;
    	}
    	else{
        	img.resize(width * resize, height * resize, img.depth(), img.spectrum(), 3);
    	}
		srcImgs[i] = img;
	}
	Stitch s(srcImgs);
	CImg<unsigned char> res = s.returnRes();
	res.display();
	res.save("res/result.bmp");
	return 0;
}