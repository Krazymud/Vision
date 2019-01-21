#include "region.h"
#include "hough.h"
#include "correct.h"
#include <string>
using namespace std;
int main(){
    string prefix = "Dataset1\\", filepath;
    cout << "Enter image path: ";
    cin >> filepath;
    Region rgn((prefix + filepath).c_str());
    CImg<unsigned char> src = rgn.getImg();
    CImg<unsigned char> rgn_res = rgn.getRes();
    Hough h(src, rgn_res);
    vector<pair<int, int>> intersects = h.getIntersects();
    Correct c(src, intersects);
    return 0;
}