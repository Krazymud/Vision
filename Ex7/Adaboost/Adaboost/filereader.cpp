#include "filereader.h"
int convertCharArrayToInt(unsigned char* array, int lengthOfArray) {
	if (lengthOfArray < 0) {
		return -1;
	}
	int res = static_cast<int>(array[0]);
	for (int i = 0; i < lengthOfArray; ++i) {
		res = (res << 8) + array[i];
	}
	return res;
}

bool isImage(unsigned char* magicNumber, int lengthOfArray) {
	int magic = convertCharArrayToInt(magicNumber, lengthOfArray);
	return magic == MAGICNUMBEROFIMAGE;
}

bool isLabel(unsigned char* magicNumber, int lengthOfArray) {
	int magic = convertCharArrayToInt(magicNumber, lengthOfArray);
	return magic == MAGICNUMBEROFLABEL;
}

cv::Mat readData(std::fstream& dataFile, int numberOfData, int dataSizeInBytes) {
	Mat dataMat;
	if (dataFile.is_open()) {
		int dataSize = dataSizeInBytes * numberOfData;
		unsigned char *tmpData = new unsigned char[dataSize];
		dataFile.read((char *)tmpData, dataSize);
		dataMat = Mat(numberOfData, dataSizeInBytes, CV_8UC1, tmpData).clone();
		delete[] tmpData;
		dataFile.close();
	}
	return dataMat;
}

cv::Mat readImageData(std::fstream& imageDataFile, int numberOfImages) {
	int imageSize = 28 * 28;
	return readData(imageDataFile, numberOfImages, imageSize);
}

cv::Mat readLabelData(std::fstream& labelDataFile, int numberOfLabel) {
	int labelSize = 1;
	return readData(labelDataFile, numberOfLabel, labelSize);
}

cv::Mat readImages(std::string& filename) {
	fstream file(filename.c_str(), ios::in | ios::binary);
	if (!file.is_open()) {
		cout << "Can't read images!" << endl;
		return Mat();
	}
	MNISTImageFileHeader fileHeader;
	file.read((char *)(&fileHeader), sizeof fileHeader);
	if (!isImage(fileHeader.magicNumber, 4)) {
		cout << "Can't read labels!" << endl;
		return Mat();
	}
	int numberOfImage = convertCharArrayToInt(fileHeader.numberOfImages, 4);
	return readImageData(file, numberOfImage);
}

cv::Mat readLabels(std::string& filename) {
	fstream file(filename.c_str(), ios::in | ios::binary);
	if (!file.is_open()) {
		return Mat();
	}
	MNISTLabelFileHeader fileHeader;
	file.read((char *)(&fileHeader), sizeof fileHeader);
	if (!isLabel(fileHeader.magicNumber, 4)) {
		return Mat();
	}
	int numberOfLabel = convertCharArrayToInt(fileHeader.numberOfLabels, 4);
	return readLabelData(file, numberOfLabel);
}