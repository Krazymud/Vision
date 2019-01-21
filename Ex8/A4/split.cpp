#include "split.h"
#include <iostream>
Split::Split(CImg<unsigned char> i, int num) {
	stringstream str;
	string image_count;
	str << num;
	str >> image_count;
	img = i;
	s = img.width() / 8;
	rate = 0.88;
	CImg<unsigned char> gray = toGray();
	cout << "Adaptive thresholding...";
	integral(gray);
	threshold(gray);
	regionGrow();
	removePoints();
	string temp;
    stringstream s;
    s << num;
    s >> temp;
    thres.save(("thres//" + temp + ".bmp").c_str());
	cout << "Done" << endl << endl;
	cout << "Spliting...";
	horiProjection();
	//img.display();
	verProjection();
	cout << "Done" << endl << endl;
	ofstream out;
	out.open("seg//nums.txt", ios::app);
	if(!out.is_open()){
		cout << "shit" << endl;
	}
	out << ver.size() << " ";
	for(int i = 0; i < ver.size(); ++i){
		out << ver[i].size();
		if(i != ver.size() - 1){
			out << " ";
		}
		string file = "seg//", temp;
		stringstream s;
		s << i;
		s >> temp;
		file = file + image_count + "-" + temp + "-";
		for(int j = 0; j < ver[i].size(); ++j){
			s.clear();
			temp.clear();
			s << j;
			s >> temp;
			ver[i][j].save((file + temp + ".bmp").c_str());
		}
	}
	out << endl << flush;
	out.close();
}

void Split::removePoints(){
    CImg<unsigned char> imgleft(thres);
    CImg<int> visited(imgleft.width(), imgleft.height(), 1, 1, 0);
    queue<pair<int, int> > mqueue;
    for(int i = 0; i < imgleft.width(); i++){
        for(int j = 0; j < imgleft.height(); j++){
            if(visited(i, j)){
                continue;
            }
            mqueue.push(make_pair(i, j));
            visited(i, j) = 1;
            vector<pair<int, int>> path;
            while(!mqueue.empty()){
                int head_i = mqueue.front().first, head_j = mqueue.front().second;
                path.push_back(make_pair(head_i, head_j));
                mqueue.pop();
                for(int x = head_i - 1; x <= head_i + 1; x++){
                    for(int y = head_j - 1; y <= head_j + 1; y++){
                        if(x < 0 || y < 0 || x >= imgleft.width() || y >= imgleft.height()){
                            continue;
                        }
                        else{
                            if(imgleft(x, y) > 0 && !visited(x, y)){
                                mqueue.push(make_pair(x, y));
                                visited(x, y) = 1;
                            }
                        }
                    }
                }
            }
            if(path.size() < 35){
                for(int x = 0; x < path.size(); x++){
                    imgleft(path[x].first, path[x].second) = 0;
                }
            }
        }
    }
	thres = imgleft;
}

int Split::getRes() {
	int i = 0;
	for (auto image : ver) {
		string file;
		stringstream s;
		s << i;
		s >> file;
		file = "temp//" + file + ".bmp";
		//image.save(file.c_str());
		++i;
	}
	return i;
}

void Split::regionGrow(){
	queue<pair<int, int>> seeds;
	seeds.push(make_pair(0, 0));
	seeds.push(make_pair(0, thres.height() - 1));
	seeds.push(make_pair(thres.width() - 1, 0));
	seeds.push(make_pair(thres.width() - 1, thres.height() - 1));
	while(!seeds.empty()){
		int x = seeds.front().first, y = seeds.front().second;
		seeds.pop();
		for(int i = x - 1; i <= x + 1; ++i){
			for(int j = y - 1; j <= y + 1; ++j){
				if(i == x && j == y) continue;
				if(i >= 0 && i < thres.width() && j >= 0 && j < thres.height()){
					if(thres(i, j) == 255){
						seeds.push(make_pair(i, j));
						thres(i, j) = 0;
					}
				}
			}
		}
	}
}

void Split::verProjection() {
	vertical = new int[thres.width()]{ 0 };
	int turn = 0;
	for (auto image : hori) {
		vector<CImg<unsigned char>> ver_;
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
		for (int i = 10; i < image.width() - 10; ++i) {
			if (vertical[i] > 0) {
				if(spliting == false){
					spliting = true;
					to_split.push_back(i - 3);
				}
			}
			else{
				if(spliting == true){
					int x = to_split[to_split.size() - 1];
					if(i - x <= 5){
						to_split.pop_back();
					}
					else{
						to_split.push_back(i + 3);
					}
					spliting = false;
				}
			}
			for (int j = 0; j < vertical[i]; ++j) {
				dis(i, j) = 255;
			}
		}
		if(spliting == true){
			to_split.push_back(image.width() - 5);
		}
		/*string file;
		stringstream s;
		s << turn;
		s >> file;
		file = "temp\\ver" + file + ".bmp";
		dis.save(file.c_str());*/
		const unsigned char color[] = { 255, 128, 64 };
		for (int i = 0; i < to_split.size(); i += 2) {
			img.draw_line(to_split[i], to_split_hor[turn], to_split[i], to_split_hor[turn + 1], color);
			img.draw_line(to_split[i + 1], to_split_hor[turn], to_split[i + 1], to_split_hor[turn + 1], color);
			CImg<unsigned char> result(to_split[i + 1] - to_split[i], to_split_hor[turn + 1] - to_split_hor[turn], 1, 1, 0);
			cimg_forXY(result, x, y) {
				result(x, y) = thres(x + to_split[i], y + to_split_hor[turn]);
			}
			result = adjust(result);
			ver_.push_back(result);
		}
		for (int i = 0; i < to_split.size(); i += 2) {
			nums.push_back(Pos(to_split[i], to_split_hor[turn]));
		}
		ver.push_back(ver_);
		turn += 2;
	}
}

CImg<unsigned char> Split::adjust(CImg<unsigned char> res){
	int width = res.width(), height = res.height();
	if(width < height){
		int fix = (height - width) / 2,
			new_width = width + 2 * fix;
		CImg<unsigned char> temp(new_width, height, 1, 1, 0);
		for(int i = fix; i < fix + width; ++i){
			for(int j = 0; j < height; ++j){
				temp(i, j) = res(i - fix, j);
			}
		}
		return temp;
	}
	return res;
}

void Split::integral(CImg<unsigned char> &gray){
	integ = CImg<int>(gray.width(), gray.height(), 1, 1, 0);
	int sum = 0;
	integ(0, 0) = gray(0, 0);
	for(int i = 1; i < gray.width(); ++i){
		integ(i, 0) = gray(i, 0) + integ(i - 1, 0);
	}
	for(int j = 1; j < gray.height(); ++j){
		sum = 0;
		for(int i = 0; i < gray.width(); ++i){
			sum += gray(i, j);
			integ(i, j) = integ(i, j - 1) + sum;
		}
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
	for (int j = 10; j < thres.height() - 10; ++j) {
		if (horizontal[j] > 3) {
			if(spliting == false){
				spliting = true;
				to_split_hor.push_back(j - 10);
			}
		}
		else{
			if(spliting == true){
				int x = to_split_hor[to_split_hor.size() - 1];
				if(j - x <= 30){
					to_split_hor.pop_back();
				}
				else{
					to_split_hor.push_back(j + 5);
				}
				spliting = false;
			}
		}
		for (int i = 0; i < horizontal[j]; ++i) {
			dis(i, j) = 255;
		}
	}
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
	thres = CImg<unsigned char>(gray.width(), gray.height(), 1, 1, 0);
	for(int i = 0; i < thres.width(); ++i){
		for(int j = 0; j < thres.height(); ++j){
			int x1 = i - (s / 2), x2 = i + (s / 2), 
				y1 = j - (s / 2), y2 = j + (s / 2);
			if(x1 < 0) x1 = 0;
			if(x2 >= thres.width()) x2 = thres.width() - 1;
			if(y1 < 0) y1 = 0;
			if(y2 >= thres.height()) y2 = thres.height() - 1;
			float average = (float)(integ(x2, y2) + integ(x1, y1) - integ(x2, y1) - integ(x1, y2)) / ((x2 - x1) * (y2 - y1));
			thres(i, j) = (gray(i, j) < average * rate) ? 255 : 0;
		}
	}
}

CImg<unsigned char> Split::toGray() {
	CImg<unsigned char>gray(img.width(), img.height(), 1, 1, 0);
	int sum = 0;
	cimg_forXY(img, x, y) {
		gray(x, y) = img(x, y, 0) * 0.299 + img(x, y, 1) * 0.587 + img(x, y, 2) * 0.114;
	}
	return gray;
}

