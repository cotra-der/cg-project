#include "cg/Bezier.h"
#include <cmath>
#include <stdexcept>
std::vector<Point> cubicBezier(PointF a,PointF b,PointF c,PointF d,int samples) {
    if(samples<1) throw std::invalid_argument("Bezier samples must be positive");
    std::vector<Point> out;
    out.reserve(samples+1);
    for(int i=0;i<=samples;++i) {
        const double t=double(i)/samples,u=1-t;
        out.push_back({int(std::lround(u*u*u*a.x+3*u*u*t*b.x+3*u*t*t*c.x+t*t*t*d.x)),
                       int(std::lround(u*u*u*a.y+3*u*u*t*b.y+3*u*t*t*c.y+t*t*t*d.y))});
    }
    return out;
}
