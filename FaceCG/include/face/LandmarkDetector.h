#pragma once
#include <memory>
#include <string>
#include <vector>
#include "core/Point.h"
// Owns the camera and inference implementation; no SDK types cross this API.
class LandmarkDetector {
public:
    LandmarkDetector(const std::string& model,int camera);
    ~LandmarkDetector();
    std::vector<PointF> next();
    static void probeCamera(int camera,int frames=60);
private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};
