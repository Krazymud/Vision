#include "filereader.h"

#include <iostream>
#include "split.h"
#include <opencv2/core.hpp>
#include <opencv2/ml.hpp>

using namespace cv;
using namespace cv::ml;

void convertToMat(int num_of_img) {
	for (int i = 0; i < num_of_img; ++i) {
		string file, temp;
		stringstream s;
		s << i;
		s >> temp;
		s.clear();
		file = "temp//" + temp + ".bmp";
		Mat img = cv::imread(file.c_str(), 0);
		resize(img, img, cv::Size(28, 28), 0, 0);
		file.clear();
		file = "temp//resized" + temp + ".jpg";
		imwrite(file.c_str(), img);
	}
}

int predict(int i) {
	string file;
	stringstream s;
	s << i;
	s >> file;
	file = "temp\\resized" + file + ".jpg";

	vector<int> digits;
	Mat img = imread(file, 0);
	Mat res = img.clone(), t;
	threshold(img, res, 150, 255, THRESH_BINARY);
	res.convertTo(t, CV_32FC1);
	Mat data(1, 28 * 28, CV_32FC1);
	for (int i = 0; i < 28; ++i) {
		for (int j = 0; j < 28; ++j) {
			data.at<float>(i * 28 + j) = t.at<float>(i, j);
		}
	}

	Ptr<Boost> model = model->load("data\\train_model.xml");
	
	int best_class = 0, var_count = data.cols, class_count = 10;
	Mat temp_sample(1, var_count + 1, CV_32F);
	double max_sum = -DBL_MAX;
	float* tptr = temp_sample.ptr<float>();
	
	const float* ptr = data.ptr<float>(0);
	for (int k = 0; k < var_count; k++)
		tptr[k] = ptr[k];
	for (int j = 0; j < class_count; j++)
	{
		tptr[var_count] = (float)j;
		float s = model->predict(temp_sample, noArray(), StatModel::RAW_OUTPUT);
		if (max_sum < s) {
			max_sum = s;
			best_class = j;
		}
	}
	return best_class;
}

void drawText(Split &sp, vector<int> &digits) {
	Mat img = imread("temp\\img.bmp");
	vector<Pos> nums = sp.getNums();
	cout << nums.size() << " " << digits.size() << endl;
	for (int i = 0; i < digits.size(); ++i) {
		stringstream s;
		string res;
		s << digits[i];
		s >> res;
		Point p(nums[i].x, nums[i].y + 10);
		putText(img, res, p, FONT_HERSHEY_COMPLEX, 0.8, Scalar(0, 255, 255));
	}

	namedWindow("win");
	imshow("win", img);
}

//识别手写数字
int main() {
	cout << "Enter the filename of img: ";
	string filename = "testImg\\", f;
	cin >> f;
	filename = filename + f;
	Split sp(CImg<unsigned char>(filename.c_str()));
	int num_of_img = sp.getRes();
	convertToMat(num_of_img);
	vector<int> digits;
	for (int i = 0; i < num_of_img; ++i) {
		int num = predict(i);
		digits.push_back(num);
	}

	drawText(sp, digits);
	waitKey(0);
	return 0;
}


//训练mnist
/*int main() {
	string data_filename = "data\\t10k-images.idx3-ubyte",
		response_filename = "data\\t10k-labels.idx1-ubyte";
	Mat data;
	(readImages(data_filename)).convertTo(data, CV_32FC1);
	Mat responses;
	(readLabels(response_filename)).convertTo(responses, CV_32SC1);
	const int class_count = 10;
	Ptr<Boost> model;
	int nsamples_all = data.rows;
	int ntrain_samples = (int)(nsamples_all * 0.01);
	int var_count = data.cols;
	Mat new_data(ntrain_samples * class_count, var_count + 1, CV_32FC1);
	Mat new_responses(ntrain_samples * class_count, 1, CV_32SC1);
	cout << "Unrolling the database...";
	for (int i = 0; i < ntrain_samples; ++i) {
		const float *data_row = data.ptr<float>(i);
		for (int j = 0; j < class_count; ++j) {
			float *new_data_row = (float *)new_data.ptr<float>(i * class_count + j);
			memcpy(new_data_row, data_row, var_count * sizeof(data_row[0]));
			new_data_row[var_count] = (float)j;
			new_responses.at<int>(i * class_count + j) = responses.at<int>(i) == j + 0;
		}
	}
	cout << "Done" << endl;
	Mat var_type(1, var_count + 2, CV_8U);
	var_type.setTo(Scalar::all(VAR_ORDERED));
	var_type.at<uchar>(var_count) = var_type.at<uchar>(var_count + 1) = VAR_CATEGORICAL;
	Ptr<TrainData> tdata = TrainData::create(new_data, ROW_SAMPLE, new_responses, noArray(), noArray(), noArray(), var_type);
	vector<float> priors(2);
	priors[0] = 1;
	priors[1] = 10;
	cout << "Training the classifier...";
	model = Boost::create();
	model->setBoostType(Boost::GENTLE);
	model->setWeakCount(5);
	model->setWeightTrimRate(0.95);
	model->setMaxDepth(5);
	model->setUseSurrogates(false);
	model->setPriors(Mat(priors));
	model->train(tdata);
	model->save("data\\model.xml");
	
	cout << "Done" << endl;
	waitKey(0);
	return 0;
}
*/


//测试
/*int main() {
	string data_filename = "data\\t10k-images.idx3-ubyte",
		response_filename = "data\\t10k-labels.idx1-ubyte";
	Mat data;
	(readImages(data_filename)).convertTo(data, CV_32FC1);
	Mat responses;
	(readLabels(response_filename)).convertTo(responses, CV_32SC1);
	const int class_count = 10;
	int nsamples_all = data.rows;
	int ntrain_samples = (int)(nsamples_all);
	cout << ntrain_samples << endl;
	int var_count = data.cols;
	Ptr<Boost> model = model->load("data\\train_model.xml");

	cout << "Predicting..." << endl;
	// 计算训练和测试数据的预测误差
	Mat temp_sample(1, var_count + 1, CV_32F);
	float* tptr = temp_sample.ptr<float>();
	double test_hr = 0;
	for (int i = 0; i < nsamples_all; i++)
	{
		int best_class = 0;
		double max_sum = -DBL_MAX;
		const float* ptr = data.ptr<float>(i);
		for (int k = 0; k < var_count; k++)
			tptr[k] = ptr[k];
		for (int j = 0; j < class_count; j++)
		{
			tptr[var_count] = (float)j;
			float s = model->predict(temp_sample, noArray(), StatModel::RAW_OUTPUT);
			
			if (max_sum < s)
			{
				max_sum = s;
				best_class = j;
			}
		}
		//判断预测是否正确（绝对值小于最小值FLT_EPSILON）
		double r = std::abs(best_class - responses.at<int>(i)) < FLT_EPSILON ? 1 : 0;
		test_hr += r;
	}
	test_hr /= nsamples_all;
	ofstream output;
	output.open("data\\predict.txt");
	if (output.is_open()) {
		output << "Recognition rate: test = " << test_hr * 100. << endl;
		output << "Number of trees: " << model->getRoots().size() << endl;
	}
	return 0;
}
*/
