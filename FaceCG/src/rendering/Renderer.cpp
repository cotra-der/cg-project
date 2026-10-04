#include "rendering/Renderer.h"
#include <GLFW/glfw3.h>
#include <stdexcept>
Renderer::Renderer(int w,int h) {
    if(!glfwInit())throw std::runtime_error("GLFW initialization failed");
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,1);
    handle=glfwCreateWindow(w,h,"FaceCG",nullptr,nullptr);
    if(!handle){glfwTerminate();throw std::runtime_error("OpenGL 2.1 window creation failed");}
    glfwMakeContextCurrent(handle);glfwSwapInterval(1);
    // Preserve short key presses that happen while camera inference is running.
    glfwSetInputMode(handle,GLFW_STICKY_KEYS,GLFW_TRUE);
}
Renderer::~Renderer(){if(handle)glfwDestroyWindow(handle);glfwTerminate();}
void Renderer::beginFrame(){
    int w,h;glfwGetFramebufferSize(handle,&w,&h);
    glViewport(0,0,w,h);glClearColor(.055f,.065f,.09f,1);glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);glLoadIdentity();glOrtho(0,w,h,0,-1,1);
    glMatrixMode(GL_MODELVIEW);glLoadIdentity();glPointSize(1);
}
void Renderer::drawPoints(const std::vector<Point>& p,const Color& c){
    glColor3f(c.r,c.g,c.b);glBegin(GL_POINTS);
    for(auto q:p)glVertex2f(q.x+.5f,q.y+.5f);
    glEnd();
}
void Renderer::endFrame(){glfwSwapBuffers(handle);glfwPollEvents();}
