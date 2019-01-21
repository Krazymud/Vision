#pragma once
#include "CImg.h"
#include <sstream>
#include <vector>
using namespace cimg_library;
using namespace std;

struct Pos {
	int x, y;
	Pos(int x_, int y_) {
		x = x_;
		y = y_;
	}
};

class Split {
private:
	CImg<unsigned char> img;
	CImg<unsigned char> thres;
	CImg<unsigned char> ver_splited;
	vector<CImg<unsigned char>> hori;
	vector<CImg<unsigned char>> ver;
	vector<int> to_split_hor;
	vector<Pos> nums;
	int *horizontal, *vertical;
public:
	Split(CImg<unsigned char> );
	CImg<unsigned char> toGray();
	void threshold(CImg<unsigned char> &);
	void horiProjection();
	void verProjection();
	int getRes();
	vector<Pos>& getNums();
};
