#ifndef CITRINE_WINDOW_H
#define CITRINE_WINDOW_H

#include <GLFW/glfw3.h>


class Window {
protected: 
    void createGlfwWindow();
public:
    GLFWwindow* glfwWindow;
    
    virtual void Loop() const;
    virtual void Close();
};


#endif //CITRINE_WINDOW_H
