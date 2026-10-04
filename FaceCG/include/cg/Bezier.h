#pragma once
#include <vector>
#include "core/Point.h"
std::vector<Point> cubicBezier(PointF p0, PointF p1, PointF p2, PointF p3, int samples = 50);
