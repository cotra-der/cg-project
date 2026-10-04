#include "cg/CatmullRom.h"
#include <cmath>
#include <stdexcept>
namespace {
Point sample(PointF a,PointF b,PointF c,PointF d,double t) {
    auto axis=[t](double a,double b,double c,double d) {
        return int(std::lround(0.5*((2*b)+(-a+c)*t+(2*a-5*b+4*c-d)*t*t+(-a+3*b-3*c+d)*t*t*t)));
    };
    return {axis(a.x,b.x,c.x,d.x),axis(a.y,b.y,c.y,d.y)};
}
std::vector<Point> curve(const std::vector<PointF>& p,int samples,bool closed) {
    if(samples<1) throw std::invalid_argument("Spline samples must be positive");
    if(p.empty()) return {};
    if(p.size()==1) return {{int(std::lround(p[0].x)),int(std::lround(p[0].y))}};
    const int n=static_cast<int>(p.size()),segments=closed?n:n-1;
    auto at=[&](int i) { return p[closed?(i+n)%n:(i<0?0:(i>=n?n-1:i))]; };
    std::vector<Point> out;
    out.reserve(segments*samples+1);
    for(int i=0;i<segments;++i)
        for(int j=0;j<samples;++j)
            out.push_back(sample(at(i-1),at(i),at(i+1),at(i+2),double(j)/samples));
    out.push_back(closed?out.front():sample(at(n-2),at(n-1),at(n-1),at(n-1),0));
    return out;
}
}
std::vector<Point> catmullRomOpen(const std::vector<PointF>& p,int s) { return curve(p,s,false); }
std::vector<Point> catmullRomClosed(const std::vector<PointF>& p,int s) { return curve(p,s,true); }
