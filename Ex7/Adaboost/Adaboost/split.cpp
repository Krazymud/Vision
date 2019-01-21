#include "split.h"
#include <iostream>
Split::Split(CImg<unsigned char> i) {
	img = i;
	CImg<unsigned char> gray = toGray();
	threshold(gray);
	//thres.display();
	horiProjection();
	//img.display();
	verProjection();
	img.display();
	img.save("temp//img.bmp");
}

int Split::getRes() {
	int i = 0;
	for (auto image : ver) {
		string file;
		stringstream s;
		s << i;
		s >> file;
		file = "temp//" + file + ".bmp";
		image.save(file.c_str());
		++i;
	}
	return i;
}

void Split::verProjection() {
	vertical = new int[thres.width()]{ 0 };
	int turn = 0;
	for (auto image : hori) {
		for (int i = 0; i < thres.width(); ++i) {
			vertical[i] = 0;
		}
		for (int i = 0; i < image.width(); ++i) {
			for (int j = 0; j < image.height(); ++j) {
				if (image(i, j) == 255) {
					++vertical[i];
				}
			}
		}
		vector<int> to_split;
		bool spliting = false;
		CImg<unsigned char> dis(image.width(), image.height(), 1, 1, 0);
		for (int i = 0; i < image.width(); ++i) {
			if (vertical[i] > 0) {
				if (spliting == false) {
					spliting = true;
					to_split.push_back(i - 5);
				}
				if (vertical[i + 1] == 0) {
					to_split.push_back(i + 5);
				}
			}
			else {
				spliting = false;
			}
			for (int j = 0; j < vertical[i]; ++j) {
				dis(i, j) = 255;
			}
		}
		string file;
		stringstream s;
		s << turn;
		s >> file;
		file = "temp\\ver" + file + ".bmp";
		dis.save(file.c_str());
		const unsigned char color[] = { 255, 128, 64 };
		for (int i = 0; i < to_split.size(); i += 2) {
			img.draw_line(to_split[i], to_split_hor[turn], to_split[i], to_split_hor[turn + 1], color);
			img.draw_line(to_split[i + 1], to_split_hor[turn], to_split[i + 1], to_split_hor[turn + 1], color);
			CImg<unsigned char> result(to_split[i + 1] - to_split[i], to_split_hor[turn + 1] - to_split_hor[turn], 1, 1, 0);
			cimg_forXY(result, x, y) {
				result(x, y) = thres(x + to_split[i], y + to_split_hor[turn]);
			}
			ver.push_back(result);
		}
		for (int i = 0; i < to_split.size(); i += 2) {
			nums.push_back(Pos(to_split[i], to_split_hor[turn]));
		}
		turn += 2;
	}
}

vector<Pos>& Split::getNums() {
	return nums;
}

void Split::horiProjection() {
	horizontal = new int[thres.height()]{ 0 };
	for (int j = 0; j < thres.height(); ++j) {
		for (int i = 0; i < thres.width(); ++i) {
			if (thres(i, j) == 255) {
				++horizontal[j];
			}
		}
	}
	bool spliting = false;
	CImg<unsigned char> dis(thres.width(), thres.height(), 1, 1, 0);
	for (int j = 0; j < thres.height(); ++j) {
		if (horizontal[j] > 0) {
			if (spliting == false) {
				spliting = true;
				to_split_hor.push_back(j - 5);
			}
			if (horizontal[j + 1] == 0) {
				to_split_hor.push_back(j + 5);
			}
		}
		else {
			spliting = false;
		}
		for (int i = 0; i < horizontal[j]; ++i) {
			dis(i, j) = 255;
		}
	}
	dis.save("temp\\hori.bmp");
	const unsigned char color[] = { 255, 128, 64 };
	for (int i = 0; i < to_split_hor.size(); i += 2) {
		img.draw_line(0, to_split_hor[i], img.width() - 1, to_split_hor[i], color);
		img.draw_line(0, to_split_hor[i + 1], img.width() - 1, to_split_hor[i + 1], color);
		CImg<unsigned char> result(thres.width(), to_split_hor[i + 1] - to_split_hor[i], 1, 1, 0);
		cimg_forXY(result, x, y) {
			result(x, y) = thres(x, y + to_split_hor[i]);
		}
		hori.push_back(result);
	}
}

void Split::threshold(CImg<unsigned char> &gray) {
	thres = CImg<unsigned char>(img.width(), img.height(), 1, 1, 0);
	cimg_forXY(gray, x, y) {
		thres(x, y) = gray(x, y) > 130 ? 0 : 255;
	}
}

CImg<unsigned char> Split::toGray() {
	CImg<unsigned char>gray(img.width(), img.height(), 1, 1, 0);
	cimg_forXY(img, x, y) {
		gray(x, y) = img(x, y, 0) * 0.299 + img(x, y, 1) * 0.587 + img(x, y, 2) * 0.114;
	}
	return gray;
}

