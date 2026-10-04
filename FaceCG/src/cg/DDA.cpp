#include "cg/DDA.h"
#include <algorithm>
#include <cmath>
#include <cstdint>
std::vector<Point> drawDDA(int x1, int y1, int x2, int y2) {
    const auto dx = int64_t(x2) - x1, dy = int64_t(y2) - y1;
    const auto steps = std::max(std::abs(dx), std::abs(dy));
    if (!steps) return {{x1,y1}};
    const double ix = double(dx)/steps, iy = double(dy)/steps;
    std::vector<Point> out;
    out.reserve(static_cast<size_t>(steps) + 1);
    for (int64_t i=0; i<=steps; ++i)
        out.push_back({int(std::lround(x1+i*ix)), int(std::lround(y1+i*iy))});
    return out;
}
