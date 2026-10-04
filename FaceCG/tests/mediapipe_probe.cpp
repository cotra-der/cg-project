#include "mediapipe/tasks/c/vision/face_landmarker/face_landmarker.h"
#include <iostream>
#include <vector>
int main(int argc,char** argv){
    MpFaceLandmarkerOptions options{};options.base_options.model_asset_path=argc>1?argv[1]:"models/face_landmarker.task";
    options.base_options.file_descriptor=-1;options.running_mode=MP_RUNNING_MODE_VIDEO;
    MpFaceLandmarkerPtr detector=nullptr;char* error=nullptr;
    auto status=MpFaceLandmarkerCreate(&options,&detector,&error);
    if(status!=kMpOk){std::cerr<<(error?error:"Model creation failed")<<'\n';if(error)MpErrorFree(error);return 1;}
    std::vector<uint8_t> blank(640*480*3);MpImagePtr image=nullptr;
    status=MpImageCreateFromUint8Data(kMpImageFormatSrgb,640,480,blank.data(),int(blank.size()),&image,&error);
    if(status!=kMpOk){if(error){std::cerr<<error<<'\n';MpErrorFree(error);}MpFaceLandmarkerClose(detector,nullptr);return 1;}
    MpFaceLandmarkerResult result{};
    status=MpFaceLandmarkerDetectForVideo(detector,image,nullptr,0,&result,&error);
    std::cout<<"Native MediaPipe model loaded; blank-frame faces="<<result.face_landmarks_count<<'\n';
    if(error){std::cerr<<error<<'\n';MpErrorFree(error);}
    MpFaceLandmarkerCloseResult(&result);MpImageFree(image);MpFaceLandmarkerClose(detector,nullptr);
    return status==kMpOk?0:1;
}
