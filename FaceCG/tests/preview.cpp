#include "face/FaceModel.h"
#include "core/Config.h"
#include <algorithm>
#include <fstream>
#include <iostream>
int main(int argc,char** argv){
    constexpr int w=Config::width,h=Config::height;
    const int mode=argc>2?std::stoi(argv[2]):6;
    auto batches=mode==6?generateFace(makeStaticFace(w,h),true):generateDemo(mode,w,h);
    std::vector<unsigned char> pixels(w*h*3);
    for(size_t i=0;i<pixels.size();i+=3){pixels[i]=14;pixels[i+1]=17;pixels[i+2]=23;}
    for(const auto& batch:batches)for(auto p:batch.points)if(p.x>=0&&p.x<w&&p.y>=0&&p.y<h){
        auto i=(p.y*w+p.x)*3;pixels[i]=static_cast<unsigned char>(std::clamp(batch.color.r,0.f,1.f)*255);
        pixels[i+1]=static_cast<unsigned char>(std::clamp(batch.color.g,0.f,1.f)*255);pixels[i+2]=static_cast<unsigned char>(std::clamp(batch.color.b,0.f,1.f)*255);
    }
    std::ofstream out(argc>1?argv[1]:"preview.ppm",std::ios::binary);
    out<<"P6\n"<<w<<' '<<h<<"\n255\n";out.write(reinterpret_cast<const char*>(pixels.data()),pixels.size());
    if(!out){std::cerr<<"Preview write failed\n";return 1;}
}
