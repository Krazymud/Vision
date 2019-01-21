#include "feature.h"

Feature::Feature(CImg<unsigned char> &gray){
    this->gray = gray;
    extractFeatures();

}
void Feature::extractFeatures(){
    CImg<unsigned char> img(gray);
    int width = img.width(), height = img.height();
    float resize = (width < height) ? (RESIZE / width) : (RESIZE / height);
    if(resize >= 1){
        resize = 1;
    }
    else{
        img.resize(width * resize, height * resize, img.depth(), img.spectrum(), 3);
    }
    vl_sift_pix *siftData = new vl_sift_pix[width * height];
    for(int i = 0; i < width; ++i){
        for(int j = 0; j < height; ++j){
            siftData[j * width + i] = img(i, j);
        }
    }

    //filter
    int octave = 4, level = 2, min = 0;
    VlSiftFilt *siftFilt = NULL;
    siftFilt = vl_sift_new(width, height, octave, level, min);

    //compute octaves one by one
    if(vl_sift_process_first_octave(siftFilt, siftData) != VL_ERR_EOF){
        while(true){
            vl_sift_detect(siftFilt);
            VlSiftKeypoint *point = siftFilt->keys;
            //each key
            for(int i = 0; i < siftFilt->nkeys; ++i){
                VlSiftKeypoint tempKeyp = *point;

                //orientations for keypoint
                double angles[4];
                int angleCount = vl_sift_calc_keypoint_orientations(siftFilt, angles, &tempKeyp);

                //each orientation
                for(int j = 0; j < angleCount; ++j){
                    double tempAngle = angles[j];
                    //128-dim
                    vl_sift_pix descriptors[128];
                    vl_sift_calc_keypoint_descriptor(siftFilt, descriptors, &tempKeyp, tempAngle);

                    vector<float> res;
                    for(int d = 0; d < 128; ++d){
                        res.push_back(descriptors[d]);
                    }
                    tempKeyp.x /= resize;
                    tempKeyp.y /= resize;
                    tempKeyp.ix = tempKeyp.x;
                    tempKeyp.iy = tempKeyp.y;

                    features.emplace(res, tempKeyp);
                }
                ++point;
            }
            if (vl_sift_process_next_octave(siftFilt) == VL_ERR_EOF) {
				break;
			}
        }
    }
    vl_sift_delete(siftFilt);
    delete[] siftData;
    siftData = NULL;
}

map<vector<float>, VlSiftKeypoint>& Feature::returnFeatures(){
    return features;
}
