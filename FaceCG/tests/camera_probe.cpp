#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <iostream>
#include <stdexcept>
int main(int argc,char** argv)try{
    const int index=argc>1?std::stoi(argv[1]):0;
    cv::VideoCapture camera(index);
    if(!camera.isOpened())throw std::runtime_error("Webcam unavailable at index "+std::to_string(index));
    for(int i=0;i<60;++i){cv::Mat frame;if(!camera.read(frame)||frame.empty())throw std::runtime_error("Webcam delivered an empty frame");}
    std::cout<<"Camera proof passed: 60 nonempty frames\n";return 0;
}catch(const std::exception& e){std::cerr<<"Camera proof failed: "<<e.what()<<'\n';return 1;}
