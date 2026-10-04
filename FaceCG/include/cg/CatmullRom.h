#pragma once
#include <vector>
#include "core/Point.h"
std::vector<Point> catmullRomOpen(const std::vector<PointF>& points, int samplesPerSegment = 4);
std::vector<Point> catmullRomClosed(const std::vector<PointF>& points, int samplesPerSegment = 4);
