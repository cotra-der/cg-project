#include "face/FaceModel.h"
#include "face/LandmarkIndices.h"
#include "cg/DDA.h"
#include "cg/Bresenham.h"
#include "cg/Bezier.h"
#include "cg/CatmullRom.h"
#include "cg/ScanlineFill.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>
namespace {
constexpr Color ink{.12f,.09f,.12f},skin{.87f,.64f,.46f},white{.96f,.96f,.92f},lip{.70f,.24f,.32f};
std::vector<Point> connect(const std::vector<Point>& p,bool dda=false) {
    std::vector<Point> out;
    for(size_t i=1;i<p.size();++i) {
        auto line=dda?drawDDA(p[i-1].x,p[i-1].y,p[i].x,p[i].y):drawBresenham(p[i-1].x,p[i-1].y,p[i].x,p[i].y);
        out.insert(out.end(),line.begin(),line.end());
    }
    return out;
}
std::vector<Point> rounded(const std::vector<PointF>& p) {
    std::vector<Point> out;
    for(auto a:p) out.push_back({int(std::lround(a.x)),int(std::lround(a.y))});
    return out;
}
std::vector<Point> pupil(const std::vector<Point>& eye) {
    if(eye.empty()) return {};
    int x0=eye[0].x,x1=x0,y0=eye[0].y,y1=y0;
    for(auto p:eye) {x0=std::min(x0,p.x);x1=std::max(x1,p.x);y0=std::min(y0,p.y);y1=std::max(y1,p.y);}
    const double cx=(x0+x1)*.5,cy=(y0+y1)*.5,r=std::min((x1-x0)*.15,(y1-y0)*.4);
    auto pixels=scanlineFill(eye);
    pixels.erase(std::remove_if(pixels.begin(),pixels.end(),[=](Point p){return (p.x-cx)*(p.x-cx)+(p.y-cy)*(p.y-cy)>r*r;}),pixels.end());
    return pixels;
}
}
FaceModel makeFaceModel(const std::vector<PointF>& p) {
    size_t expected=0;for(const auto& g:FaceLandmarks::GROUPS)expected+=g.size();
    if(p.size()!=expected) throw std::invalid_argument("Selected landmark count mismatch");
    FaceModel f;
    std::vector<PointF>* features[]={&f.faceOval,&f.leftEye,&f.rightEye,&f.leftEyebrow,&f.rightEyebrow,&f.outerLips,&f.innerLips,&f.noseBridge,&f.noseBottom};
    size_t offset=0;
    for(size_t i=0;i<FaceLandmarks::GROUPS.size();++i) {
        size_t count=FaceLandmarks::GROUPS[i].size();
        features[i]->assign(p.begin()+offset,p.begin()+offset+count);offset+=count;
    }
    return f;
}
FaceModel makeStaticFace(int w,int h,bool mirror) {
    FaceModel f;
    for(int i=0;i<16;++i) { const double a=i*6.283185307179586/16; f.faceOval.push_back({float(.5+.28*std::sin(a)),float(.49-.39*std::cos(a))}); }
    f.leftEye={{.29f,.40f},{.32f,.37f},{.38f,.37f},{.42f,.40f},{.38f,.43f},{.32f,.43f}};
    f.rightEye={{.58f,.40f},{.62f,.37f},{.68f,.37f},{.71f,.40f},{.68f,.43f},{.62f,.43f}};
    f.leftEyebrow={{.28f,.33f},{.32f,.28f},{.38f,.29f},{.43f,.33f}};
    f.rightEyebrow={{.57f,.33f},{.62f,.29f},{.68f,.28f},{.72f,.33f}};
    f.noseBridge={{.50f,.42f},{.48f,.53f},{.50f,.57f}};
    f.noseBottom={{.44f,.57f},{.50f,.60f},{.56f,.57f}};
    f.outerLips={{.38f,.69f},{.44f,.65f},{.5f,.66f},{.56f,.65f},{.62f,.69f},{.56f,.73f},{.5f,.74f},{.44f,.73f}};
    f.innerLips={{.41f,.69f},{.46f,.685f},{.5f,.69f},{.54f,.685f},{.59f,.69f},{.54f,.71f},{.5f,.715f},{.46f,.71f}};
    for(auto* feature:{&f.faceOval,&f.leftEye,&f.rightEye,&f.leftEyebrow,&f.rightEyebrow,&f.outerLips,&f.innerLips,&f.noseBridge,&f.noseBottom})
        for(auto& p:*feature) {p.x=mirror?w-1-p.x*w:p.x*w;p.y*=h;}
    return f;
}
std::vector<PointBatch> generateFace(const FaceModel& f,bool fill) {
    const auto oval=catmullRomClosed(f.faceOval,6),le=catmullRomClosed(f.leftEye),re=catmullRomClosed(f.rightEye),mouth=catmullRomClosed(f.outerLips),inside=catmullRomClosed(f.innerLips);
    std::vector<PointBatch> out;
    if(fill) {out.push_back({scanlineFill(oval),skin});out.push_back({scanlineFill(le),white});out.push_back({scanlineFill(re),white});out.push_back({scanlineFill(mouth),lip});out.push_back({scanlineFill(inside),ink});}
    out.push_back({pupil(le),ink});out.push_back({pupil(re),ink});
    for(const auto* p:{&oval,&le,&re})out.push_back({connect(*p),ink});
    for(const auto* b:{&f.leftEyebrow,&f.rightEyebrow})
        if(b->size()==4)out.push_back({connect(cubicBezier((*b)[0],(*b)[1],(*b)[2],(*b)[3])),ink});
    out.push_back({connect(rounded(f.noseBridge),true),ink});out.push_back({connect(rounded(f.noseBottom)),ink});
    out.push_back({connect(mouth),ink});out.push_back({connect(inside),ink});
    return out;
}
std::vector<PointBatch> generateDemo(int mode,int w,int h) {
    const Color cyan{.25f,.85f,.9f},pink{1.f,.4f,.6f};
    std::vector<PointBatch> out;
    if(mode==1) {
        // One unmistakable DDA example: shallow positive slope.
        out.push_back({drawDDA(int(w*.16),int(h*.72),int(w*.84),int(h*.28)),cyan});
    } else if(mode==2) {
        // One unmistakable Bresenham example: steep negative slope.
        out.push_back({drawBresenham(int(w*.28),int(h*.18),int(w*.66),int(h*.84)),pink});
    } else if(mode==3) {
        std::vector<PointF> p={{w*.15f,h*.7f},{w*.3f,h*.05f},{w*.65f,h*.95f},{w*.85f,h*.3f}};
        out.push_back({connect(rounded(p),true),pink});out.push_back({connect(cubicBezier(p[0],p[1],p[2],p[3])),cyan});
    } else if(mode==4) {
        out.push_back({connect(catmullRomOpen({{w*.1f,h*.3f},{w*.3f,h*.1f},{w*.5f,h*.4f},{w*.9f,h*.2f}},20)),cyan});
        out.push_back({connect(catmullRomClosed({{w*.3f,h*.65f},{w*.5f,h*.5f},{w*.7f,h*.7f},{w*.5f,h*.9f}},20)),pink});
    } else {
        std::vector<Point> p={{w/5,h/5},{4*w/5,h/5},{w/2,h/2},{4*w/5,4*h/5},{w/5,4*h/5}};
        out.push_back({scanlineFill(p),cyan});p.push_back(p.front());out.push_back({connect(p),pink});
    }
    return out;
}
