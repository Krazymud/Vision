#pragma once
#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>

using namespace std;
using cv::Mat;

const int MAGICNUMBEROFIMAGE = 2051;
const int MAGICNUMBEROFLABEL = 2049;

struct MNISTImageFileHeader {
	unsigned char magicNumber[4];
	unsigned char numberOfImages[4];
	unsigned char numberOfRows[4];
	unsigned char numberOfCols[4];
};

struct MNISTLabelFileHeader {
	unsigned char magicNumber[4];
	unsigned char numberOfLabels[4];
};

int convertCharArrayToInt(unsigned char*, int);

bool isImage(unsigned char*, int);

bool isLabel(unsigned char*, int);

cv::Mat readData(std::fstream&, int, int);

cv::Mat readImageData(std::fstream&, int);

cv::Mat readLabelData(std::fstream&, int);

cv::Mat readImages(std::string&);

cv::Mat readLabels(std::string&);
