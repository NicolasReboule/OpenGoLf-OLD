//
// Created by nicol on 26/05/2024.
//

#include "Window.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb-master/stb_image.h"

std::unique_ptr<Window> Window::_instance = nullptr;

Window::Window(int w, int h) {
    _width = w;
    _height = h;

    glm::vec3 viewPoint(DEFAULT_VIEW_POINT[0], DEFAULT_VIEW_POINT[1], DEFAULT_VIEW_POINT[2]);
    glm::vec3 viewCenter(DEFAULT_VIEW_CENTER[0], DEFAULT_VIEW_CENTER[1], DEFAULT_VIEW_CENTER[2]);
    glm::vec3 upVector(DEFAULT_UP_VECTOR[0], DEFAULT_UP_VECTOR[1], DEFAULT_UP_VECTOR[2]);

    float aspect = static_cast<float>(_width) / static_cast<float>(_height);
    _viewer = std::make_unique<Viewer>(viewPoint, viewCenter, upVector, 45.0f, aspect);
    init();
}

std::unique_ptr<Window> &Window::getInstance(int w, int h) {
    if (_instance == nullptr) {
        auto *window = new Window(w, h);
        _instance = std::unique_ptr<Window>(window);
    }
    return _instance;
}

void Window::init() {
    initSkyBox();
}

void Window::draw() {
    glViewport(0, 0, _width, _height);
    glEnable(GL_DEPTH_TEST); // enable depth testing
    glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"

    glm::vec3 eye = _viewer->getViewPoint();
    glm::vec3 look = _viewer->getViewCenter();
    glm::vec3 up = _viewer->getUpVector();
    glm::mat4 view = lookAt(eye, look, up);
    if (_skyBox) {
        drawSkyBox(view, eye);
    }
}

void Window::setSize(int w, int h) {
    _width = w;
    _height = h;
}

void Window::setAspect(float aspect) {
    _viewer->setAspectRatio(aspect);
}

glm::mat4 Window::lookAt(glm::vec3 campos, glm::vec3 look, glm::vec3 up) {
    glm::vec3 z = glm::normalize(campos - look);
    glm::vec3 x = glm::normalize(glm::cross(up, z));
    glm::vec3 y = glm::cross(z, x);

    glm::mat4 view = glm::mat4(1.0f);
    view[0][0] = x.x;
    view[1][0] = x.y;
    view[2][0] = x.z;
    view[0][1] = y.x;
    view[1][1] = y.y;
    view[2][1] = y.z;
    view[0][2] = z.x;
    view[1][2] = z.y;
    view[2][2] = z.z;
    view[3][0] = -glm::dot(x, campos);
    view[3][1] = -glm::dot(y, campos);
    view[3][2] = -glm::dot(z, campos);
    return view;
}

glm::mat4 Window::perspective(float fovY, float aspect, float nearZ, float farZ) {
    float f = 1.0f / glm::tan(glm::radians(fovY) / 2.0f);
    glm::mat4 result = glm::mat4(0.0f);
    result[0][0] = f / aspect;
    result[1][1] = f;
    result[2][2] = (farZ + nearZ) / (nearZ - farZ);
    result[2][3] = -1.0f;
    result[3][2] = (2.0f * farZ * nearZ) / (nearZ - farZ);
    return result;
}

void Window::resize(GLFWwindow* glWindow, int width, int height) {
    std::unique_ptr<Window> &window = getInstance();
    window->setSize(width, height);
    window->setAspect((float) width / (float) height);
}

void Window::keyEvents(GLFWwindow* glWindow, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(glWindow, GLFW_TRUE);
}

void Window::updateCursor(GLFWwindow* glWindow, double x_pos, double y_pos) {
    std::unique_ptr<Window> &window = getInstance();
    window->setCursorPos(static_cast<float>(x_pos), static_cast<float>(y_pos));
}

void Window::mouseEvents(GLFWwindow* glWindow, int button, int action, int mods) {
    std::unique_ptr<Window> &window = getInstance();
    if (action == GLFW_PRESS) {
        double x_pos, y_pos;
        glfwGetCursorPos(glWindow, &x_pos, &y_pos);
        window->setLastCursorPos(static_cast<float>(x_pos), static_cast<float>(y_pos));
    }
    switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT:
            if (action == GLFW_PRESS)
                window->setLeftButtonDown(true);
            else if (action == GLFW_RELEASE)
                window->setLeftButtonDown(false);
            break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            if (action == GLFW_PRESS)
                window->setRightButtonDown(true);
            else if (action == GLFW_RELEASE)
                window->setRightButtonDown(false);
            break;
        case GLFW_MOUSE_BUTTON_MIDDLE:
            if (action == GLFW_PRESS)
                window->setMiddleButtonDown(true);
            else if (action == GLFW_RELEASE)
                window->setMiddleButtonDown(false);
            break;
        default:
            break;
    }
}

void Window::mouseDragging() {
    std::unique_ptr<Window> &window = getInstance();
    std::unique_ptr<Viewer> &viewer = window->getViewer();
    glm::vec2 cursor_pos = window->getCursorPos();
    glm::vec2 last_cursor_pos = window->getLastCursorPos();
    int width = window->getWidth();
    int height = window->getHeight();

    if (window->getLeftButtonDown()) {
        float fractionChangeX = static_cast<float>(cursor_pos.x - last_cursor_pos.x) / static_cast<float>(width);
        float fractionChangeY = static_cast<float>(last_cursor_pos.y - cursor_pos.y) / static_cast<float>(height);
        viewer->rotate(fractionChangeX, fractionChangeY);
    }
    else if (window->getMiddleButtonDown()) {
        float fractionChangeX = (cursor_pos.x - last_cursor_pos.x) / static_cast<float>(width);
        float fractionChangeY = (last_cursor_pos.y - cursor_pos.y) / static_cast<float>(height);
        viewer->zoom(fractionChangeY);
    }
    else if (window->getRightButtonDown()) {
        float fractionChangeX = (cursor_pos.x - last_cursor_pos.x) / static_cast<float>(width);
        float fractionChangeY = (last_cursor_pos.y - cursor_pos.y) / static_cast<float>(height);
        viewer->translate(-fractionChangeX, -fractionChangeY, true);
    }
    last_cursor_pos.x = cursor_pos.x;
    last_cursor_pos.y = cursor_pos.y;
}

void Window::setCursorPos(float x, float y) {
    _cursor_pos.x = x;
    _cursor_pos.y = y;
}

void Window::setLastCursorPos(float x, float y) {
    _last_cursor_pos.x = x;
    _last_cursor_pos.y = y;
}

void Window::initSkyBox() {
    _skyBox = std::make_unique<SkyBox>();
    _shaderSkyBox = std::make_unique<ShaderProgram>();
    _shaderSkyBox->initFromFiles("shaders/skybox.vert", "shaders/skybox.frag");

    _shaderSkyBox->addUniform("DrawSkyBox"); //This is boolean variable for special use, more in later

    _shaderSkyBox->addUniform("MVP");
    _shaderSkyBox->addUniform("WorldCameraPosition"); //Camera position in the world space
    _shaderSkyBox->addUniform("ModelMatrix"); //Model Matrix for converting from local to world

    _shaderSkyBox->addUniform("CubeMapTex"); //cubemap texture
    _shaderSkyBox->addUniform("MaterialColor"); //Object color
    _shaderSkyBox->addUniform("ReflectFactor"); //Ratio of mixup the objectcolor with cubemap color
}

void Window::drawSkyBox(glm::mat4 view, glm::vec3 eye) {
    glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(1.0, 0.0, 0.0));
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.5, 0.5, 0.5));
    glm::mat4 model = translate * rotate * scale; // Combination of transformation matrix

    glm::mat4 projectionSkyBox = perspective(45.0f, 1.0f * static_cast<float>(_width) / static_cast<float>(_height), 0.1f, 10000.0f);
    glm::mat4 skyboxView = glm::mat4(glm::mat3(view));
    glm::mat4 mvp = projectionSkyBox * skyboxView * model;

    _shaderSkyBox->use();

    glUniformMatrix4fv(_shaderSkyBox->uniform("ModelMatrix"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(_shaderSkyBox->uniform("MVP"), 1, GL_FALSE, glm::value_ptr(mvp));

    glUniform1i(_shaderSkyBox->uniform("CubeMapTex"), 0);

    glUniform3fv(_shaderSkyBox->uniform("WorldCameraPosition"), 1, glm::value_ptr(eye));
    glm::vec4 materialColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    glUniform4fv(_shaderSkyBox->uniform("MaterialColor"), 1, glm::value_ptr(materialColor));
    glUniform1f(_shaderSkyBox->uniform("ReflectFactor"), 0.2f);

    glDepthMask(GL_FALSE);
    if (_skyBox) {
        glUniform1i(_shaderSkyBox->uniform("DrawSkyBox"), GL_TRUE); //indicate that we are doing SKybox
        _skyBox->draw();
    }
    glDepthMask(GL_TRUE);
    _shaderSkyBox->disable();
}
