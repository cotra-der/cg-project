#include "face/LandmarkDetector.h"
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include "mediapipe/tasks/c/vision/face_landmarker/face_landmarker.h"
#include <algorithm>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <stdexcept>
// Verified against the ctypes declarations shipped in official MediaPipe 1.0.1.
#if defined(_WIN64)
static_assert(sizeof(MpBaseOptions)==72 && sizeof(MpFaceLandmarkerOptions)==104,
              "MediaPipe 1.0.1 option ABI mismatch");
static_assert(sizeof(MpNormalizedLandmark)==40 && sizeof(MpFaceLandmarkerResult)==48,
              "MediaPipe 1.0.1 result ABI mismatch");
#endif
namespace {
void check(MpStatus status,char* error,const char* context) {
    std::string message=error?error:"Unknown MediaPipe error";
    if(error)MpErrorFree(error);
    if(status!=kMpOk)throw std::runtime_error(std::string(context)+": "+message);
}
struct ImageOwner {MpImagePtr image=nullptr;~ImageOwner(){if(image)MpImageFree(image);}};
struct ResultOwner {MpFaceLandmarkerResult result{};~ResultOwner(){MpFaceLandmarkerCloseResult(&result);}};
}
struct LandmarkDetector::Impl {
    cv::VideoCapture camera;
    MpFaceLandmarkerPtr detector=nullptr;
    int64_t timestamp=-1;
    std::chrono::steady_clock::time_point start=std::chrono::steady_clock::now();
    ~Impl(){if(detector){char* error=nullptr;MpFaceLandmarkerClose(detector,&error);if(error){std::cerr<<error<<'\n';MpErrorFree(error);}}}
};
LandmarkDetector::LandmarkDetector(const std::string& model,int camera):impl(std::make_unique<Impl>()) {
    if(!std::filesystem::is_regular_file(model))throw std::runtime_error("MediaPipe model missing: "+model);
    MpFaceLandmarkerOptions options{};
    options.base_options.model_asset_path=model.c_str();
    options.base_options.file_descriptor=-1;
    options.base_options.delegate=MP_DELEGATE_CPU;
    options.running_mode=MP_RUNNING_MODE_VIDEO;
    options.num_faces=1;
    char* error=nullptr;
    auto status=MpFaceLandmarkerCreate(&options,&impl->detector,&error);
    check(status,error,"MediaPipe initialization failed");
    if(!impl->camera.open(camera))throw std::runtime_error("Webcam unavailable");
    impl->camera.set(cv::CAP_PROP_FRAME_WIDTH,640);impl->camera.set(cv::CAP_PROP_FRAME_HEIGHT,480);
}
LandmarkDetector::~LandmarkDetector()=default;
std::vector<PointF> LandmarkDetector::next(){
    cv::Mat bgr,rgb;
    if(!impl->camera.read(bgr)||bgr.empty())throw std::runtime_error("Webcam stopped delivering frames");
    cv::cvtColor(bgr,rgb,cv::COLOR_BGR2RGB);
    if(!rgb.isContinuous())rgb=rgb.clone();
    ImageOwner image;char* error=nullptr;
    auto status=MpImageCreateFromUint8Data(kMpImageFormatSrgb,rgb.cols,rgb.rows,rgb.ptr<uint8_t>(),int(rgb.total()*rgb.elemSize()),&image.image,&error);
    check(status,error,"MediaPipe image conversion failed");
    auto elapsed=std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now()-impl->start).count();
    impl->timestamp=std::max(impl->timestamp+1,int64_t(elapsed));
    ResultOwner result;error=nullptr;
    status=MpFaceLandmarkerDetectForVideo(impl->detector,image.image,nullptr,impl->timestamp,&result.result,&error);
    check(status,error,"MediaPipe inference failed");
    if(!result.result.face_landmarks_count)return {};
    const auto& landmarks=result.result.face_landmarks[0];
    std::vector<PointF> out;out.reserve(landmarks.landmarks_count);
    for(uint32_t i=0;i<landmarks.landmarks_count;++i)out.push_back({landmarks.landmarks[i].x,landmarks.landmarks[i].y});
    return out;
}
void LandmarkDetector::probeCamera(int index,int frames){
    cv::VideoCapture camera(index);if(!camera.isOpened())throw std::runtime_error("Webcam unavailable");
    for(int i=0;i<frames;++i){cv::Mat frame;if(!camera.read(frame)||frame.empty())throw std::runtime_error("Empty webcam frame");}
    std::cout<<"Camera proof passed: "<<frames<<" nonempty frames\n";
}
