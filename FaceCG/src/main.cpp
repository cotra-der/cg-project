#include "rendering/Renderer.h"
#include "face/LandmarkMapper.h"
#include "face/LandmarkSmoother.h"
#include "core/Config.h"
#ifdef FACECG_ENABLE_LIVE
#include "face/LandmarkDetector.h"
#endif
#include <GLFW/glfw3.h>
#include <array>
#include <chrono>
#include <cmath>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
int main(int argc,char** argv) try {
    bool live=false,probeGL=false,probeCamera=false,probeLandmarks=false,fill=true,mirror=true;
    int camera=0,frameLimit=0;
    std::string model="models/face_landmarker.task";
    for(int i=1;i<argc;++i){std::string arg=argv[i];
        if(arg=="--live")live=true;else if(arg=="--static")live=false;
        else if(arg=="--probe-gl")probeGL=true;
        else if(arg=="--probe-camera")probeCamera=true;
        else if(arg=="--probe-landmarks")probeLandmarks=true;
        else if(arg=="--model"&&i+1<argc)model=argv[++i];
        else if(arg=="--camera"&&i+1<argc)camera=std::stoi(argv[++i]);
        else if(arg=="--frames"&&i+1<argc)frameLimit=std::stoi(argv[++i]);
        else if(arg=="--help"){std::cout<<"FaceCG [--static|--live] [--model path] [--camera index] [--frames N]\n--probe-gl | --probe-camera | --probe-landmarks\n1-5: algorithms, 6: face, F: fill, M: mirror, S: static, C: camera, Esc: exit\n";return 0;}
        else throw std::runtime_error("Unknown or incomplete option: "+arg);
    }
#ifdef FACECG_ENABLE_LIVE
    if(probeCamera){LandmarkDetector::probeCamera(camera);return 0;}
    std::unique_ptr<LandmarkDetector> detector;
    if(live||probeLandmarks)detector=std::make_unique<LandmarkDetector>(model,camera);
    if(probeLandmarks){
        std::vector<PointF> previous;bool changed=false;int detected=0;
        for(int i=0;i<150;++i){auto p=detector->next();if(p.empty())continue;
            auto selected=mapSelectedLandmarks(p,Config::width,Config::height,false);
            ++detected;std::cout<<"Face detected; selected landmark: x="<<selected[0].x<<" y="<<selected[0].y<<'\n';
            if(previous.size()==selected.size())for(size_t j=0;j<selected.size();++j)if(std::abs(previous[j].x-selected[j].x)>1||std::abs(previous[j].y-selected[j].y)>1)changed=true;
            previous=selected;
        }
        if(!detected||!changed)throw std::runtime_error("Landmark proof incomplete: need a visible moving face");
        return 0;
    }
#else
    (void)camera;(void)model;
    if(live||probeCamera||probeLandmarks)throw std::runtime_error("This is a static-only build. Enable FACECG_ENABLE_LIVE with OpenCV and MediaPipe; no substitute detector is provided.");
#endif
    Renderer renderer(Config::width,Config::height);
    auto window=renderer.window();int mode=6,lastW=0,lastH=0,frames=0;bool dirty=true;
    LandmarkSmoother smoother(Config::smoothing);
    std::array<bool,GLFW_KEY_LAST+1> held{};
    auto pressed=[&](int key){bool down=glfwGetKey(window,key)==GLFW_PRESS;bool edge=down&&!held[key];held[key]=down;return edge;};
    std::vector<PointBatch> batches;
    std::string status="Static";
    auto fpsStart=std::chrono::steady_clock::now();int fpsFrames=0;
    while(!glfwWindowShouldClose(window)){
        if(pressed(GLFW_KEY_ESCAPE))break;
        for(int key=GLFW_KEY_1;key<=GLFW_KEY_6;++key)if(pressed(key)){mode=key-GLFW_KEY_1+1;dirty=true;}
        if(pressed(GLFW_KEY_F)){fill=!fill;dirty=true;}
        if(pressed(GLFW_KEY_M)){mirror=!mirror;smoother.reset();dirty=true;}
        if(pressed(GLFW_KEY_S)){live=false;mode=6;status="Static";smoother.reset();dirty=true;
#ifdef FACECG_ENABLE_LIVE
            detector.reset();
#endif
        }
        if(pressed(GLFW_KEY_C)){
#ifdef FACECG_ENABLE_LIVE
            try{if(!detector)detector=std::make_unique<LandmarkDetector>(model,camera);live=true;mode=6;smoother.reset();dirty=true;}catch(const std::exception& e){status=e.what();std::cerr<<status<<'\n';}
#else
            status="Camera unavailable: static-only build";std::cerr<<status<<'\n';
#endif
        }
        int w,h;glfwGetFramebufferSize(window,&w,&h);
        if(w<=0||h<=0){glfwWaitEventsTimeout(.05);continue;}
        if(w!=lastW||h!=lastH){dirty=true;smoother.reset();lastW=w;lastH=h;}
        if(probeGL)batches={{{{100,100}},{1,1,1}}};
        else if(mode<6){if(dirty)batches=generateDemo(mode,w,h);}
        else if(!live){if(dirty)batches=generateFace(makeStaticFace(w,h,mirror),fill);}
#ifdef FACECG_ENABLE_LIVE
        else {
            try{auto landmarks=detector->next();if(landmarks.empty()){batches.clear();smoother.reset();status="Live: no face";}
                else{batches=generateFace(makeFaceModel(smoother.smooth(mapSelectedLandmarks(landmarks,w,h,mirror))),fill);status="Live";}}
            catch(const std::exception& e){batches.clear();smoother.reset();status=e.what();std::cerr<<status<<'\n';live=false;detector.reset();status+=" | Press S for static or C to retry";}
        }
#endif
        dirty=false;renderer.beginFrame();for(const auto& batch:batches)renderer.drawPoints(batch.points,batch.color);
        if(probeGL&&frames==0){unsigned char pixel[3]={};glReadPixels(100,h-1-100,1,1,GL_RGB,GL_UNSIGNED_BYTE,pixel);if(pixel[0]<240)throw std::runtime_error("OpenGL point readback failed");std::cout<<"OpenGL proof passed: white pixel at (100,100)\n";}
        renderer.endFrame();++frames;++fpsFrames;
        const auto elapsed=std::chrono::duration<double>(std::chrono::steady_clock::now()-fpsStart).count();
        if(elapsed>=.5||frames==1){std::string title="FaceCG | "+status+" | Mode "+std::to_string(mode)+" | "+std::to_string(int(fpsFrames/elapsed))+" FPS | 1-6 F M S C Esc";glfwSetWindowTitle(window,title.c_str());fpsStart=std::chrono::steady_clock::now();fpsFrames=0;}
        if(frameLimit>0&&frames>=frameLimit)break;
    }
    return 0;
}catch(const std::exception& e){std::cerr<<"FaceCG: "<<e.what()<<'\n';return 1;}
