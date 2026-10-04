#pragma once
#include <vector>
#include "core/Point.h"
Point mapLandmarkToPixel(float x,float y,int width,int height,bool mirrorX);
std::vector<PointF> mapSelectedLandmarks(const std::vector<PointF>& normalized,int width,int height,bool mirrorX);
