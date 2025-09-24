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

    GLFWwindow *gl_window = glfwCreateWindow(windowSettings.width, windowSettings.height, windowSettings.title, nullptr, nullptr);

    if (!gl_window)
    {
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(gl_window);

    if (gl3wInit()) {
        fprintf(stderr, "Failed to initialize OpenGL\n");
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

    glfwSetWindowTitle(gl_window, windowSettings.title);
    glfwSetWindowSizeCallback(gl_window, window->resize);
    glfwSetMouseButtonCallback(gl_window, window->mouseEvents);
    glfwSetCursorPosCallback(gl_window, window->updateCursor);
    glfwSetKeyCallback(gl_window, window->keyEvents);

    while (!glfwWindowShouldClose(gl_window))
    {
        glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glfwPollEvents();
        window->update();
        window->draw();
        glfwSwapBuffers(gl_window);
    }
    glfwDestroyWindow(gl_window);
    glfwTerminate();
}

int main() {
    window();
}