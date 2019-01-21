#include "morph.h"

Morph::Morph(const char *img1, const char *img2, int a, int p, int b, int mode){
    cimg::imagemagick_path("G:\\ImageMagick-7.0.8-Q16\\convert.exe");
    this->mode = mode;
    this->a = a;
    this->p = p;
    this->b = b;
    srcImg = CImg<unsigned char>(img1);
    desImg = CImg<unsigned char>(img2);
    RecordPoints();
    InitLines();
    for(frameIndex = 0; frameIndex < frameCount; ++frameIndex){
        WarpAndMorph();
        cout << "\r" << flush;
        cout << "Percentage: " << (double)(frameIndex + 1) / (frameCount + 1);
    }
}

void Morph::RecordPoints(){
    CImg<double> from(srcImg), to(desImg);
    int width = from.width();
    from.append(to);
    CImgDisplay disp(from,"Draw Points");
    int turn = 0;
    while(!disp.is_closed()){
        disp.wait();
        if(disp.button() & 1){
            int x0 = disp.mouse_x(), y0 = disp.mouse_y();
            if(x0 < width){
                srcPoints.push_back(make_pair(x0, y0));
                from.draw_circle(x0, y0, 3, color).display(disp);
                if(turn == 1){
                    int size = srcPoints.size();
                    from.draw_line(srcPoints[size - 1].first, srcPoints[size - 1].second, srcPoints[size - 2].first, srcPoints[size-2].second, color).display(disp);
                }
                turn = (turn == 1) ? 0 : 1;
            }
            else{
                desPoints.push_back(make_pair(x0 - width, y0));
                from.draw_circle(x0, y0, 3, color).display(disp);
                if(turn == 1){
                    int size = desPoints.size();
                    from.draw_line(desPoints[size - 1].first + width, desPoints[size - 1].second, desPoints[size - 2].first + width, desPoints[size-2].second, color).display(disp);
                }
                turn = (turn == 1) ? 0 : 1;
            }
            cout << srcPoints.size() << " " << desPoints.size() << endl;
        }
    }
    cout << srcPoints.size() << " " << desPoints.size() << endl;
}

void Morph::InitLines(){
    for(int i = 0; i < srcPoints.size(); i += 2){
        Line l(srcPoints[i], srcPoints[i + 1]);
        Line r(desPoints[i], desPoints[i + 1]);
        LinePair lp(l, r);
        linePairs.push_back(lp);
    }
}

double Line::Getu(pair<int, int> x){
    double xres = (x.first - P.first) * (Q.first - P.first);
    double yres = (x.second - P.second) * (Q.second - P.second);
    double res = (xres + yres) / (len * len);
    return res;
}

double Line::Getv(pair<int, int> x){
    double xres = (x.first - P.first) * (Q.second - P.second);
    double yres = (x.second - P.second) * (P.first - Q.first);
    double res = (xres + yres) / len;
    return res;
}

double Line::GetLen(){
    return len;
}

void LinePair::GetWarpLine(){
    for(int i = 0; i < frameCount; ++i){
        double ratio = (double)(i + 1) / (frameCount + 1);
        Line l;
        l.P.first = (1 - ratio) * leftLine.P.first + ratio * rightLine.P.first;
        l.P.second = (1 - ratio) * leftLine.P.second + ratio * rightLine.P.second;
        l.Q.first = (1 - ratio) * leftLine.Q.first + ratio * rightLine.Q.first;
        l.Q.second = (1 - ratio) * leftLine.Q.second + ratio * rightLine.Q.second;
        l.len = sqrt((l.P.first - l.Q.first) * (l.P.first - l.Q.first) + (l.P.second - l.Q.second) * (l.P.second - l.Q.second));
        warpLine.push_back(l);
    }
}

double Line::GetDist(double u, double v, pair<int, int> x){
    if(u >= 0 && u <= 1){
        return abs(v);
    }
    if(u < 0){
        return sqrt((x.first - P.first) * (x.first - P.first) + (x.second - P.second) * (x.second - P.second));
    }
    if(u > 1){
        return sqrt((x.first - Q.first) * (x.first - Q.first) + (x.second - Q.second) * (x.second - Q.second));
    }
    return 0;
}

pair<double, double> Line::GetCorrPoint(double u, double v){
    double xres = P.first + u * (Q.first - P.first) + v * (Q.second - P.second) / len;
    double yres = P.second + u * (Q.second - P.second) + v * (P.first - Q.first) / len;
    return make_pair(xres, yres);
}

void Morph::WarpAndMorph(){
    double ratio = (double)(1 + frameIndex)/(1 + frameCount);
    int width = srcImg.width(), height = srcImg.height();
    int linecount = linePairs.size();
    CImg<unsigned char> res(srcImg);
    cimg_forXY(srcImg, x, y){
        pair<double, double> dsumL(0, 0);
        pair<double, double> dsumR(0, 0);
        double weightsumL = 0, weightsumR = 0;
        for(int i = 0; i < linecount; ++i){
            //left
            Line srcLine = linePairs[i].leftLine;
            Line dstLine = linePairs[i].warpLine[frameCount - frameIndex - 1];
            double u = srcLine.Getu(make_pair(x, y));
            double v = srcLine.Getv(make_pair(x, y));
            pair<double, double> corrx = dstLine.GetCorrPoint(u, v);
            pair<double, double> d(corrx.first - x, corrx.second - y);
            double weight = pow((pow(srcLine.GetLen(), p) / (a + srcLine.GetDist(u, v, make_pair(x, y)))), b);
            dsumL.first += d.first * weight;
            dsumL.second += d.second * weight;
            weightsumL += weight;
            //right
            srcLine = linePairs[i].rightLine;
            u = srcLine.Getu(make_pair(x, y));
            v = srcLine.Getv(make_pair(x, y));
            corrx = dstLine.GetCorrPoint(u, v);
            d = make_pair(corrx.first - x, corrx.second - y);
            weight = pow((pow(srcLine.GetLen(), p) / (a + srcLine.GetDist(u, v, make_pair(x, y)))), b);
            dsumR.first += d.first * weight;
            dsumR.second += d.second * weight;
            weightsumR += weight;
        }
        double rL = x + dsumL.first / weightsumL, cL = y + dsumL.second / weightsumL;
        double rR = x + dsumR.first / weightsumR, cR = y + dsumR.second / weightsumR;
        if(rL < 0) rL = 0;
        if(cL < 0) cL = 0;
        if(rL >= width) rL = width - 1;
        if(cL >= height) cL = height - 1;
        if(rR < 0) rR = 0;
        if(cR < 0) cR = 0;
        if(rR >= width) rR = width - 1;
        if(cR >= height) cR = height - 1;
        CImg<unsigned char> colorL = Bilinear(rL, cL, desImg);
        CImg<unsigned char> colorR = Bilinear(rR, cR, srcImg);
        if(mode == 0){
            res(x, y) = ratio * colorL(0, 0) + (1 - ratio) * colorR(0, 0);
        }
        else{
            res(x, y, 0) = ratio * colorL(0, 0, 0) + (1 - ratio) * colorR(0, 0, 0);
            res(x, y, 1) = ratio * colorL(0, 0, 1) + (1 - ratio) * colorR(0, 0, 1);
            res(x, y, 2) = ratio * colorL(0, 0, 2) + (1 - ratio) * colorR(0, 0, 2);
        }
        
    }
    char filename[20];
    sprintf(filename, "%d.jpg", frameIndex + 2);
    res.save_jpeg(filename);
}

CImg<unsigned char> Morph::Bilinear(double r, double c, CImg<unsigned char> des){
    if(mode == 0){
        CImg<unsigned char> res(1, 1, 1, 1, 0);
        double a0 = r - floor(r), b0 = c - floor(c), fr = floor(r), fc = floor(c), cr = ceil(r), cc = ceil(c);
        if(cr > des.width() - 1) cr = des.width() - 1;
        if(cc > des.height() - 1) cr = des.height() - 1;
        res(0, 0) = (1 - a0) * (1 - b0) * des(fr, fc) + a0 * (1 - b0) * des(cr, fc) + (1 - a0) * b0 * des(fr, cc) + a0 * b0 * des(cr, cc);
        return res;
    }
    else{
        CImg<unsigned char> res(1, 1, 1, 3, 0);
        double a0 = r - floor(r), b0 = c - floor(c), fr = floor(r), fc = floor(c), cr = ceil(r), cc = ceil(c);
        if(cr > des.width() - 1) cr = des.width() - 1;
        if(cc > des.height() - 1) cr = des.height() - 1;
        res(0, 0, 0) = (1 - a0) * (1 - b0) * des(fr, fc, 0) + a0 * (1 - b0) * des(cr, fc, 0) + (1 - a0) * b0 * des(fr, cc, 0) + a0 * b0 * des(cr, cc, 0);
        res(0, 0, 1) = (1 - a0) * (1 - b0) * des(fr, fc, 1) + a0 * (1 - b0) * des(cr, fc, 1) + (1 - a0) * b0 * des(fr, cc, 1) + a0 * b0 * des(cr, cc, 1);
        res(0, 0, 2) = (1 - a0) * (1 - b0) * des(fr, fc, 2) + a0 * (1 - b0) * des(cr, fc, 2) + (1 - a0) * b0 * des(fr, cc, 2) + a0 * b0 * des(cr, cc, 2);
        return res;
    }
}