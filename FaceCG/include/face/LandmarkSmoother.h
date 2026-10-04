#pragma once
#include <vector>
#include "core/Point.h"
class LandmarkSmoother {
public:
    explicit LandmarkSmoother(float alpha=0.4f);
    std::vector<PointF> smooth(const std::vector<PointF>& current);
    void reset() { previous.clear(); }
private:
    float alpha;
    std::vector<PointF> previous;
};
