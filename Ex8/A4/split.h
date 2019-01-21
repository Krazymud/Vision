#pragma once

#include "CImg.h"
#include <sstream>
#include <fstream>
#include <queue>
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
	int s;
	float rate;
	CImg<unsigned char> img;
	CImg<unsigned char> thres;
	CImg<int> integ;
	CImg<unsigned char> ver_splited;
	vector<CImg<unsigned char>> hori;
	vector<vector<CImg<unsigned char>>> ver;
	vector<int> to_split_hor;
	vector<Pos> nums;
	int *horizontal, *vertical;
public:
	Split(CImg<unsigned char>, int);
	CImg<unsigned char> toGray();
	void threshold(CImg<unsigned char> &);
	void horiProjection();
	CImg<unsigned char> adjust(CImg<unsigned char>);
	void verProjection();
	void integral(CImg<unsigned char> &);
	void regionGrow();
	void removePoints();
	int getRes();
	vector<Pos>& getNums();
};