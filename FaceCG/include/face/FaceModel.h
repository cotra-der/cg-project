#pragma once
#include <vector>
#include "core/Point.h"
#include "core/Color.h"
struct FaceModel {
    std::vector<PointF> faceOval,leftEye,rightEye,leftEyebrow,rightEyebrow;
    std::vector<PointF> outerLips,innerLips,noseBridge,noseBottom;
};
struct PointBatch { std::vector<Point> points; Color color; };
FaceModel makeStaticFace(int width,int height,bool mirror=false);
FaceModel makeFaceModel(const std::vector<PointF>& selected);
std::vector<PointBatch> generateFace(const FaceModel& face,bool fill);
std::vector<PointBatch> generateDemo(int mode,int width,int height);
