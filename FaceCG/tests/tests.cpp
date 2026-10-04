#include "cg/DDA.h"
#include "cg/Bresenham.h"
#include "cg/Bezier.h"
#include "cg/CatmullRom.h"
#include "cg/ScanlineFill.h"
#include "face/FaceModel.h"
#include "face/LandmarkMapper.h"
#include "face/LandmarkSmoother.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <set>
#include <stdexcept>
namespace {
int checks=0;
void check(bool condition,const char* name){++checks;if(!condition)throw std::runtime_error(name);}
template<class F>void throws(F f,const char* name){bool threw=false;try{f();}catch(const std::invalid_argument&){threw=true;}check(threw,name);}
void lineTest(const std::vector<Point>& p,Point a,Point b){
    check(p.front()==a&&p.back()==b,"Line endpoints");
    check(p.size()==size_t(std::max(std::abs(a.x-b.x),std::abs(a.y-b.y))+1),"Line pixel count");
    const int dx=b.x-a.x,dy=b.y-a.y;
    for(auto q:p)check(2*std::abs(dy*(q.x-a.x)-dx*(q.y-a.y))<=std::max(std::abs(dx),std::abs(dy)),"Line stays within half a pixel of ideal line");
    for(size_t i=1;i<p.size();++i)check(std::abs(p[i].x-p[i-1].x)<=1&&std::abs(p[i].y-p[i-1].y)<=1,"Line connectivity");
}
// Independent ray-casting oracle at pixel centers for fill coverage.
bool inside(const std::vector<Point>& p,double x,double y){
    bool result=false;
    for(size_t i=0,j=p.size()-1;i<p.size();j=i++)
        if((p[i].y>y)!=(p[j].y>y)&&x<(p[j].x-p[i].x)*(y-p[i].y)/double(p[j].y-p[i].y)+p[i].x)result=!result;
    return result;
}
void fillTest(const std::vector<Point>& polygon){
    const auto fill=scanlineFill(polygon);std::set<std::pair<int,int>> pixels;
    for(auto p:fill)pixels.emplace(p.x,p.y);
    check(pixels.size()==fill.size(),"Fill has no duplicate pixels");
    for(int y=-2;y<=12;++y)for(int x=-2;x<=12;++x)
        check(bool(pixels.count({x,y}))==inside(polygon,x+.5,y+.5),"Fill coverage vs ray casting");
}
}
int main()try{
    for(int x=-8;x<=8;++x)for(int y=-8;y<=8;++y){
        lineTest(drawDDA(0,0,x,y),{0,0},{x,y});lineTest(drawBresenham(0,0,x,y),{0,0},{x,y});
        lineTest(drawBresenham(x,y,0,0),{x,y},{0,0});
    }
    auto b=cubicBezier({0,0},{0,8},{8,8},{8,0},2);
    check(b.front()==Point{0,0}&&b.back()==Point{8,0}&&b[1]==Point{4,6},"Bezier endpoints and analytic midpoint");
    throws([]{cubicBezier({},{},{},{},0);},"Invalid Bezier samples");
    const std::vector<PointF> controls={{1,1},{9,1},{9,9},{1,9}};
    auto open=catmullRomOpen(controls,8),closed=catmullRomClosed(controls,8);
    check(open.front()==Point{1,1}&&open.back()==Point{1,9},"Open spline endpoints");
    check(closed.front()==closed.back()&&closed.size()==33,"Closed spline seam");
    for(size_t i=0;i<controls.size();++i)check(closed[i*8]==Point{int(controls[i].x),int(controls[i].y)},"Spline interpolation");
    check(catmullRomClosed({}).empty(),"Empty spline");
    check(catmullRomOpen({{2,3}}).size()==1,"Single spline point");
    throws([]{catmullRomOpen({},0);},"Invalid spline samples");
    fillTest({{0,0},{10,0},{10,10},{0,10}});
    fillTest({{0,0},{10,0},{5,10}});
    fillTest({{0,0},{10,0},{10,4},{4,4},{4,10},{0,10}});
    fillTest({{0,10},{4,10},{4,4},{10,4},{10,0},{0,0},{0,10}});
    check(scanlineFill({}).empty()&&scanlineFill({{0,0},{1,0},{2,0}}).empty(),"Degenerate fill");
    check(mapLandmarkToPixel(1,1,800,700,false)==Point{799,699},"Map boundary clamp");
    check(mapLandmarkToPixel(0,.5f,800,700,true)==Point{799,350},"Mirror");
    check(mapLandmarkToPixel(-1,2,800,700,false)==Point{0,699},"Out of view clamp");
    throws([]{mapLandmarkToPixel(0,0,0,700,false);},"Invalid viewport");
    throws([]{mapSelectedLandmarks({},800,700,false);},"Invalid landmark count");
    LandmarkSmoother smoother(.4f);smoother.smooth({{0,0}});
    auto smoothed=smoother.smooth({{10,20}});check(smoothed[0].x==4&&smoothed[0].y==8,"EMA");
    smoother.reset();check(smoother.smooth({{10,20}})[0].x==10,"EMA reset");
    throws([]{LandmarkSmoother bad(0);},"Invalid EMA alpha");
    auto selected=mapSelectedLandmarks(std::vector<PointF>(478,{.5f,.5f}),800,700,false);
    check(!makeFaceModel(selected).faceOval.empty(),"Selection and model integration");
    throws([]{makeFaceModel({});},"Model rejects incomplete features");
    const auto face=generateFace(makeStaticFace(800,700),true);
    check(face.size()>=10&&!face.front().points.empty(),"Static face generated and filled");
    for(int mode=1;mode<=5;++mode)check(!generateDemo(mode,800,700).empty(),"Demo geometry generated");
    generateFace({},true);
    std::cout<<checks<<" checks passed\n";return 0;
}catch(const std::exception& e){std::cerr<<"Test failure: "<<e.what()<<'\n';return 1;}
