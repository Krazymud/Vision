#include "stitch.h"

CImg<unsigned char> Stitch::toGrayImage(CImg<unsigned char> &img){
    if (img.spectrum() == 1) {
		return img;
	}
    CImg<unsigned char> res(img.width(), img.height(), img.depth(), 1, 0);
    cimg_forXY(img, x, y){
        res(x, y) = 0.299 * img(x, y, 0) + 0.587 * img(x, y, 1) + 0.114 * img(x, y, 2);
    }
    return res;
}

Stitch::Stitch(vector<CImg<unsigned char>> & srcImgs){
    this->srcImgs = srcImgs;
    stitching();
}

int getMiddleIndex(vector<vector<int>> &matchIndex) {
	int one_side = 0;

	for (int i = 0; i < matchIndex.size(); i++) {
		if (matchIndex[i].size() == 1) {
			one_side = i;
			break;
		}
	}

	int middleIndex = one_side;
	int preMiddleIndex = -1;
	int n = matchIndex.size() / 2;
	
	while (n--) {
		for (int i = 0; i < matchIndex[middleIndex].size(); i++) {
			if (matchIndex[middleIndex][i] != preMiddleIndex) {
				preMiddleIndex = middleIndex;
				middleIndex = matchIndex[middleIndex][i];

				break;
			}
		}
	}

	return middleIndex;
}

CImg<unsigned char> Stitch::returnRes(){
    return result;
}

void updateByHomo(map<vector<float>, VlSiftKeypoint> &feature, Homoparam p, float x, float y){
    for(auto &f : feature){
        float x_ = f.second.x;
        float y_ = f.second.y;
        f.second.x = xInSrc(x_, y_, p) - x;
        f.second.y = yInSrc(x_, y_, p) - y;
        f.second.ix = int(f.second.x);
        f.second.iy = int(f.second.y);
    }
}
void updateByOffset(map<vector<float>, VlSiftKeypoint> &feature, int x, int y){
    for(auto &f : feature){
        f.second.x -= x;
        f.second.y -= y;
        f.second.ix = int(f.second.x);
        f.second.iy = int(f.second.y);
    }
}
void homoWarping(CImg<unsigned char> &src, CImg<unsigned char> &dst, Homoparam p, float x, float y){
	for (int dst_x = 0; dst_x < dst.width(); dst_x++) {
		for (int dst_y = 0; dst_y < dst.height(); dst_y++) {
			int src_x = xInSrc(dst_x + x, dst_y + y, p);
			int src_y = yInSrc(dst_x + x, dst_y + y, p);

			if (src_x >= 0 && src_x < src.width() && src_y >= 0 && src_y < src.height()) {
				for (int k = 0; k < src.spectrum(); k++) {
					dst(dst_x, dst_y, k) = bilinear_interpolation(src, src_x, src_y, k);
                }
			}
		}
	}
}
void offsetMoving(CImg<unsigned char> &src, CImg<unsigned char> &dst, float x, float y){
	for (int dst_x = 0; dst_x < dst.width(); dst_x++) {
		for (int dst_y = 0; dst_y < dst.height(); dst_y++) {
			int src_x = dst_x + x;
			int src_y = dst_y + y;

			if (src_x >= 0 && src_x < src.width() && src_y >= 0 && src_y < src.height()) {
				for (int k = 0; k < src.spectrum(); k++) {
					dst(dst_x, dst_y, k) = src(src_x, src_y, k);
				}
			}
		}
	}
}
void Stitch::stitching(){
    const unsigned char red[] = { 255,0,0 };
    vector<map<vector<float>, VlSiftKeypoint>> features(srcImgs.size());
    for(int i = 0; i < srcImgs.size(); ++i){
        cout << "Preprocessing image " << i << " ..." << endl;
        cout << "Spherical warping started..." << endl;
        srcImgs[i] = cylinderWarping(srcImgs[i]);
        cout << "Spherical warping finished." << endl;
        CImg<unsigned char> gray = toGrayImage(srcImgs[i]);
        cout << "SIFT features extracting..." << endl;
        Feature f(gray);
        features[i] = f.returnFeatures();
        cout << "SIFT features extracted." << endl;
    }
    
    cout << "Start to find adjacent images..." << endl;

    Matching m(srcImgs, features);
    vector<vector<int>> matchIndex = m.getMatchIndex();
    
    
    cout << "Start to stitch those images..." << endl;
    int start = getMiddleIndex(matchIndex), prev = start;
    queue<int> unstitched;
    unstitched.push(start);
    result = srcImgs[start];

    while(!unstitched.empty()){
        int index = unstitched.front();
        unstitched.pop();
        for(int i = matchIndex[index].size() - 1; i >= 0; --i){
            int dest = matchIndex[index][i];
            if(!m.isStitchNeed(index, dest)){
                continue;
            }
            else{
                m.removeStitch(index, dest);
                unstitched.push(dest);
            }
            
            vector<matching_points> srcToDst = m.findFromFeatures(features[index], features[dest]);
            vector<matching_points> dstToSrc = m.findFromFeatures(features[dest], features[index]);
            
            if(srcToDst.size() > dstToSrc.size()){
                dstToSrc.clear();
                for(int i = 0; i < srcToDst.size(); ++i){
                    matching_points temp(srcToDst[i].dst, srcToDst[i].src);
                    dstToSrc.push_back(temp);
                }
            }
            else{
                srcToDst.clear();
                for(int n = 0; n < dstToSrc.size(); ++n){
                    matching_points temp(dstToSrc[n].dst, dstToSrc[n].src);
                    srcToDst.push_back(temp);
                }
            }

            Homoparam forward = m.ransac(dstToSrc);
            Homoparam backward = m.ransac(srcToDst);

            vector<float> sideLength = getSideLength(srcImgs[dest], forward, result.width(), result.height());
            int newWidth = ceil(sideLength[0] - sideLength[1]), newHeight = ceil(sideLength[2] - sideLength[3]);
            CImg<unsigned char> blend_1(newWidth, newHeight, 1, srcImgs[dest].spectrum(), 0);
            CImg<unsigned char> blend_2(newWidth, newHeight, 1, srcImgs[dest].spectrum(), 0);

            homoWarping(srcImgs[dest], blend_1, backward, sideLength[1], sideLength[3]);
            offsetMoving(result, blend_2, sideLength[1], sideLength[3]);
            updateByHomo(features[dest], forward, sideLength[1], sideLength[3]);
            updateByOffset(features[prev], sideLength[1], sideLength[3]);

            Blend b(blend_1, blend_2);
            prev = dest;
            result = b.returnRes();
            cout << "Image " << dest << " has been stitched." << endl;
        }
    }
}

vector<float> Stitch::getSideLength(CImg<unsigned char> &img, Homoparam p, int width, int height){
    float min_x = xInSrc(0, 0, p), min_y = yInSrc(0, 0, p), max_x = min_x, max_y = min_y;
    if(xInSrc(img.width() - 1, 0, p) < min_x){
        min_x = xInSrc(img.width() - 1, 0, p);
    }
    if(xInSrc(0, img.height() - 1, p) < min_x){
        min_x = xInSrc(0, img.height() - 1, p);
    }
    if(xInSrc(img.width() - 1, img.height() - 1, p) < min_x){
        min_x = xInSrc(img.width() - 1, img.height() - 1, p);
    }
    //min_x
    if(yInSrc(img.width() - 1, 0, p) < min_y){
        min_y = yInSrc(img.width() - 1, 0, p);
    }
    if(yInSrc(0, img.height() - 1, p) < min_y){
        min_y = yInSrc(0, img.height() - 1, p);
    }
    if(yInSrc(img.width() - 1, img.height() - 1, p) < min_y){
        min_y = yInSrc(img.width() - 1, img.height() - 1, p);
    }
    //min_y
    if(xInSrc(img.width() - 1, 0, p) > max_x){
        max_x = xInSrc(img.width() - 1, 0, p);
    }
    if(xInSrc(0, img.height() - 1, p) > max_x){
        max_x = xInSrc(0, img.height() - 1, p);
    }
    if(xInSrc(img.width() - 1, img.height() - 1, p) > max_x){
        max_x = xInSrc(img.width() - 1, img.height() - 1, p);
    }
    //max_x
    if(yInSrc(img.width() - 1, 0, p) > max_y){
        max_y = yInSrc(img.width() - 1, 0, p);
    }
    if(yInSrc(0, img.height() - 1, p) > max_y){
        max_y = yInSrc(0, img.height() - 1, p);
    }
    if(yInSrc(img.width() - 1, img.height() - 1, p) > max_y){
        max_y = yInSrc(img.width() - 1, img.height() - 1, p);
    }
    //max_y
    min_x = (min_x < 0) ? min_x : 0;
    min_y = (min_y < 0) ? min_y : 0;
    max_x = (max_x >= width) ? max_x : width;
    max_y = (max_y >= height) ? max_y : height;
    vector<float> res;
    res.push_back(max_x);
    res.push_back(min_x);
    res.push_back(max_y);
    res.push_back(min_y);
    return res;
}