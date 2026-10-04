#pragma once
#include "face/FaceModel.h"
struct GLFWwindow;
class Renderer {
public:
    Renderer(int width,int height);
    ~Renderer();
    Renderer(const Renderer&)=delete;
    Renderer& operator=(const Renderer&)=delete;
    GLFWwindow* window() const {return handle;}
    void beginFrame();
    void drawPoints(const std::vector<Point>& points,const Color& color);
    void endFrame();
private:
    GLFWwindow* handle=nullptr;
};
