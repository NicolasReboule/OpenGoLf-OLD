//
// Created by nicol on 26/05/2024.
//

#include <iostream>
#include "Window.hpp"
#include "GL/gl3w.h"
#include "GLFW/glfw3.h"
#include "cyclone.h"

constexpr struct WindowSettings {
    int width = 800;
    int height = 600;
    const char *title = "OpenGL FrameWork";
} windowSettings;

static void window()
{
    if (!glfwInit())
    {
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *glWindow = glfwCreateWindow(windowSettings.width, windowSettings.height, windowSettings.title, nullptr, nullptr);

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

    std::unique_ptr<Window> &window = Window::getInstance(windowSettings.width, windowSettings.height);

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
        window->mouseDragging();
    }
    glfwDestroyWindow(glWindow);
    glfwTerminate();
}

int main() {
    window();
    return 0;
}