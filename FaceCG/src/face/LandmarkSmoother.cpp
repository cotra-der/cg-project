#include "face/LandmarkSmoother.h"
#include <cmath>
#include <stdexcept>
LandmarkSmoother::LandmarkSmoother(float a):alpha(a) {
    if(!std::isfinite(a)||a<=0||a>1) throw std::invalid_argument("Smoothing alpha must be in (0,1]");
}
std::vector<PointF> LandmarkSmoother::smooth(const std::vector<PointF>& p) {
    if(previous.size()!=p.size()) previous=p;
    else for(size_t i=0;i<p.size();++i) {
        previous[i].x=alpha*p[i].x+(1-alpha)*previous[i].x;
        previous[i].y=alpha*p[i].y+(1-alpha)*previous[i].y;
    }
    return previous;
}
