#include "cg/Bresenham.h"
#include <algorithm>
#include <cstdint>
#include <cstdlib>
std::vector<Point> drawBresenham(int x1, int y1, int x2, int y2) {
    const int64_t dx=std::abs(int64_t(x2)-x1), dy=-std::abs(int64_t(y2)-y1);
    const int sx=x1<x2?1:-1, sy=y1<y2?1:-1;
    int64_t err=dx+dy;
    std::vector<Point> out;
    out.reserve(static_cast<size_t>(std::max(dx,-dy))+1);
    for (;;) {
        out.push_back({x1,y1});
        if(x1==x2 && y1==y2) break;
        // Both decisions use the old error so every octant is handled.
        const auto twice=2*err;
        if(twice>=dy) { err+=dy; x1+=sx; }
        if(twice<=dx) { err+=dx; y1+=sy; }
    }
    return out;
}
