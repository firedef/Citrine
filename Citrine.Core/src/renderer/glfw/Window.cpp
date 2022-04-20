#include "Window.h"

void Window::Loop() const {
    while (!glfwWindowShouldClose(glfwWindow)) glfwPollEvents();
}

void Window::Close() {
    glfwDestroyWindow(glfwWindow);
    glfwTerminate();
}

void Window::createGlfwWindow() {
    glfwWindow = glfwCreateWindow(800, 600, "unnamed window", nullptr, nullptr);
    glfwMakeContextCurrent(glfwWindow);
    glfwSwapInterval(0);
}
