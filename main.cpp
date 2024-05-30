//
// Created by nicol on 26/05/2024.
//

#include <iostream>
#include "Window.hpp"
#include "GL/gl3w.h"
#include "GLFW/glfw3.h"
#include "cyclone.h"

void window()
{
    if (!glfwInit())
    {
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    int width = 800;
    int height = 800;
    GLFWwindow *glWindow = glfwCreateWindow(width, height, "OpenGL FrameWork", NULL, NULL);

    if (!glWindow)
    {
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(glWindow);

    if (gl3wInit()) {
        fprintf(stderr, "failed to initialize OpenGL\n");
        return;
    }

    if (!gl3wIsSupported(4, 6)) {
        fprintf(stderr, "OpenGL 4.6 not supported\n");
        return;
    }

    printf("OpenGL %s, GLSL %s\n", glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));
    glfwSwapInterval(1);
    printf("OpenGL %s, GLSL %s\n", glGetString(GL_VERSION),glGetString(GL_SHADING_LANGUAGE_VERSION));

    std::unique_ptr<Window> &window = Window::getInstance(width, height);

    glfwSetWindowTitle(glWindow, "MyOpenGLWindow");
    glfwSetWindowSizeCallback(glWindow, window->resize);
    glfwSetMouseButtonCallback(glWindow, window->mouseEvents);
    glfwSetCursorPosCallback(glWindow, window->updateCursor);
    glfwSetKeyCallback(glWindow, window->keyEvents);

    while (!glfwWindowShouldClose(glWindow))
    {
        glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        window->draw();
        glfwSwapBuffers(glWindow);
        glfwPollEvents();
        window->mouseDragging(width, height);
    }

    glfwDestroyWindow(glWindow);
    glfwTerminate();
}

int main() {
    cyclone::Vector3 v1(1, 2, 3);
    std::cout << "Vector 1: " << v1.toString() << std::endl;
    window();
    return 0;
}