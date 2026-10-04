#include "face/LandmarkMapper.h"
#include "face/LandmarkIndices.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>
Point mapLandmarkToPixel(float x,float y,int w,int h,bool mirror) {
    if(w<=0 || h<=0 || !std::isfinite(x) || !std::isfinite(y))
        throw std::invalid_argument("Invalid landmark or viewport");
    int px=std::min(w-1,int(std::clamp(x,0.f,1.f)*w));
    int py=std::min(h-1,int(std::clamp(y,0.f,1.f)*h));
    return {mirror?w-1-px:px,py};
}
std::vector<PointF> mapSelectedLandmarks(const std::vector<PointF>& p,int w,int h,bool mirror) {
    std::vector<PointF> out;
    for(const auto& group:FaceLandmarks::GROUPS) for(int i:group) {
        if(size_t(i)>=p.size()) throw std::invalid_argument("Invalid MediaPipe landmark count");
        auto q=mapLandmarkToPixel(p[i].x,p[i].y,w,h,mirror);
        out.push_back({float(q.x),float(q.y)});
    }
    return out;
}
