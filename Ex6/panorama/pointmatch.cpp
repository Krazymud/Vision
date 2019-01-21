#include "pointmatch.h"
Matching::Matching(vector<CImg<unsigned char>> &srcImgs, vector<map<vector<float>, VlSiftKeypoint>> &features){
    this->srcImgs = srcImgs;
    this->features = features;
    matchIndex = vector<vector<int>>(srcImgs.size());
    findAdjacent();
}

void Matching::findAdjacent(){
    for(int i = 0; i < srcImgs.size(); ++i){
        for(int j = 0; j < srcImgs.size(); ++j){
            if(i == j) continue;
            vector<matching_points> pairs = findFromFeatures(features[i], features[j]);
            if(pairs.size() >= 20){
                toStitch[i][j] = true;
                cout << "Image " << i << " and " << j << " are adjacent." << endl;
                matchIndex[i].push_back(j);
            }
        }
    }
}

vector<vector<int>>& Matching::getMatchIndex(){
    return matchIndex;
}

bool Matching::isStitchNeed(int src, int dst){
    return toStitch[src][dst];
}

void Matching::removeStitch(int src, int dst){
    toStitch[src][dst] = false;
    toStitch[dst][src] = false;
}

vector<matching_points> Matching::findFromFeatures(map<vector<float>, VlSiftKeypoint> &f1, map<vector<float>, VlSiftKeypoint> &f2){
    VlKDForest *kdForest = vl_kdforest_new(VL_TYPE_FLOAT, 128, 1, VlDistanceL1);
    float *data = new float[128 * f1.size()];
    int k = 0;
    for(auto f : f1){
        const vector<float> &descriptors = f.first;
        for(int i = 0; i < 128; ++i){
            data[i + 128 * k] = descriptors[i];
        }
        ++k;
    }
    vl_kdforest_build(kdForest, f1.size(), data);
    vector<matching_points> result;
    VlKDForestSearcher *searcher = vl_kdforest_new_searcher(kdForest);
    VlKDForestNeighbor neighbours[2];
    for(auto f : f2){
        float *temp = new float[128];
        for(int i = 0; i < 128; ++i){
            temp[i] = (f.first)[i];
        }
        int visited = vl_kdforestsearcher_query(searcher, neighbours, 2, temp);
        float ratio = neighbours[0].distance / neighbours[1].distance;
        if(ratio < 0.5){
            vector<float> des(128);
            for(int i = 0; i < 128; ++i){
                des[i] = data[i + neighbours[0].index * 128];
            }
            VlSiftKeypoint l = f1.find(des)->second;
            VlSiftKeypoint r = f.second;
            result.push_back(matching_points(l, r));
        }
        delete[] temp;
        temp = NULL;
    }
    vl_kdforestsearcher_delete(searcher);
    vl_kdforest_delete(kdForest);
    delete[] data;
    data = NULL;
    return result;
}

int Matching::random(int min, int max){
    return rand() % (max - min + 1) + min;
}

Homoparam Matching::ransac(vector<matching_points> &pairs){
    srand(time(0));
    //p = 0.99; w = 0.5; n = 4;
    int iter = ceil(log(1 - RATE) / log(1 - pow(INLIER_RATIO, NUM_OF_PAIR)));
    vector<int> maxInliers;
    for(int i = 0; i < iter; ++i){
        vector<matching_points> ran_pairs;
        set<int> indexes;
        for(int j = 0; j < NUM_OF_PAIR; ++j){ //4 pairs
            int index = random(0, pairs.size() - 1);
            while(indexes.find(index) != indexes.end()){
                index = random(0, pairs.size() - 1);
            }
            indexes.insert(index);
            ran_pairs.push_back(pairs[index]);
        }

        Homoparam p = getHomography(ran_pairs);
        vector<int> temp_inliers = getInliers(pairs, p, indexes);
        if(temp_inliers.size() > maxInliers.size()){
            maxInliers = temp_inliers;
        }
    }
    Homoparam res = leastSquare(pairs, maxInliers);
    return res;
}

Homoparam Matching::leastSquare(vector<matching_points> &pairs, vector<int> &inliers){
    int size = inliers.size();
    CImg<double> A(4, size, 1, 1, 0);
    CImg<double> b(1, size, 1, 1, 0);

    for(int i = 0; i < size; ++i){
        int index = inliers[i];
        A(0, i) = pairs[index].src.x;
		A(1, i) = pairs[index].src.y;
		A(2, i) = pairs[index].src.x * pairs[index].src.y;
		A(3, i) = 1;

		b(0, i) = pairs[index].dst.x;
    }
    CImg<double> x1 = b.get_solve(A);

	for (int i = 0; i < size; i++) {
		int index = inliers[i];
		b(0, i) = pairs[index].dst.y;
	}
	CImg<double> x2 = b.get_solve(A);
    return Homoparam(x1(0, 0), x1(0, 1), x1(0, 2), x1(0, 3), x2(0, 0), x2(0, 1), x2(0, 2), x2(0, 3));
}

vector<int> Matching::getInliers(vector<matching_points> &pairs, Homoparam p, set<int> &indexes){
    vector<int> inliers;
    for(int i = 0; i < pairs.size(); ++i){
        if(indexes.find(i) != indexes.end()){
            continue;
        }
        float x = pairs[i].dst.x;
        float y = pairs[i].dst.y;
        float x_ = xInSrc(pairs[i].src.x, pairs[i].src.y, p);
        float y_ = yInSrc(pairs[i].src.x, pairs[i].src.y, p);

        float dist = sqrt(pow(x_ - x, 2) + pow(y_ - y, 2));
        if(dist < INLIER_LIMIT){
            inliers.push_back(i);
        }
    }
    return inliers;
}

float xInSrc(float x, float y, Homoparam H) {
	return H.a * x + H.b * y + H.c * x * y + H.d;
}

float yInSrc(float x, float y, Homoparam H) {
	return H.e * x + H.f * y + H.g * x * y + H.h;
}

Homoparam Matching::getHomography(vector<matching_points> &pairs){
    CImg<float> A(4, NUM_OF_PAIR, 1, 1, 0);
    CImg<float> b(1, NUM_OF_PAIR, 1, 1, 0);
    //calc Ax = b
    for(int i = 0; i < NUM_OF_PAIR; ++i){
        A(0, i) = pairs[i].src.x;
        A(1, i) = pairs[i].src.y;
        A(2, i) = pairs[i].src.x * pairs[i].src.y;
        A(3, i) = 1;
        b(0, i) = pairs[i].dst.x;
    }
    CImg<float> x1 = b.get_solve(A);
    for(int i = 0; i < NUM_OF_PAIR; ++i){
        b(0, i) = pairs[i].dst.y;
    }
    CImg<float> x2 = b.get_solve(A);
    return Homoparam(x1(0,0), x1(0,1), x1(0,2), x1(0,3), x2(0,0), x2(0,1), x2(0,2), x2(0,3));
}