//
// Created by nicol on 26/05/2024.
//

#include "Window.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb-master/stb_image.h"

std::unique_ptr<Window> Window::_instance = nullptr;
bool Window::run = false;
bool Window::_should_push_ball = false;

Window::Window(int w, int h) {
    _last_frame = std::chrono::steady_clock::now();
    _width = w;
    _height = h;
    glm::vec3 viewPoint(DEFAULT_VIEW_POINT[0], DEFAULT_VIEW_POINT[1], DEFAULT_VIEW_POINT[2]);
    glm::vec3 viewCenter(DEFAULT_VIEW_CENTER[0], DEFAULT_VIEW_CENTER[1], DEFAULT_VIEW_CENTER[2]);
    glm::vec3 upVector(DEFAULT_UP_VECTOR[0], DEFAULT_UP_VECTOR[1], DEFAULT_UP_VECTOR[2]);

    float aspect = static_cast<float>(_width) / static_cast<float>(_height);
    _viewer = std::make_unique<Viewer>(viewPoint, viewCenter, upVector, 45.0f, aspect);

    _light = Light {
        glm::vec4(10.0f, 10.0f, 0.0f, 1.0f),
        glm::vec3(0.9f, 0.9f, 0.9f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(0.1f, 0.1f, 0.1f),
        glm::vec3(0.9f, 0.9f, 0.9f),
        glm::vec3(0.9f, 0.9f, 0.9f)
    };
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
    _gravity = std::make_shared<cyclone::ParticleGravity>(cyclone::Vector3::GRAVITY);
    _skybox = std::make_unique<SkyBox>();
    _ball = std::make_shared<Ball>(
            0.5,
            cyclone::Vector3(0, 20, 0),
            cyclone::Vector3(0, 0, 0),
            1.0,
            "assets/golf_ball.jpg");
    _max_contacts = 1;
    _world = std::make_unique<cyclone::World>(_max_contacts, _max_contacts * 2);
    _world->firstBody = new cyclone::World::BodyRegistration(_ball->body, nullptr);

    std::array<cyclone::Vector3, 24> cube = {
            // Front
            cyclone::Vector3(-1.0, -1.0, 1.0),
            cyclone::Vector3(1.0, -1.0, 1.0),
            cyclone::Vector3(1.0, 1.0, 1.0),
            cyclone::Vector3(-1.0, 1.0, 1.0),
            // Right
            cyclone::Vector3(1.0, -1.0, 1.0),
            cyclone::Vector3(1.0, -1.0, -1.0),
            cyclone::Vector3(1.0, 1.0, -1.0),
            cyclone::Vector3(1.0, 1.0, 1.0),
            // Back
            cyclone::Vector3(-1.0, -1.0, -1.0),
            cyclone::Vector3(-1.0, 1.0, -1.0),
            cyclone::Vector3(1.0, 1.0, -1.0),
            cyclone::Vector3(1.0, -1.0, -1.0),
            // Left
            cyclone::Vector3(-1.0, -1.0, 1.0),
            cyclone::Vector3(-1.0, 1.0, 1.0),
            cyclone::Vector3(-1.0, 1.0, -1.0),
            cyclone::Vector3(-1.0, -1.0, -1.0),
            // Bottom
            cyclone::Vector3(-1.0, -1.0, 1.0),
            cyclone::Vector3(-1.0, -1.0, -1.0),
            cyclone::Vector3(1.0, -1.0, -1.0),
            cyclone::Vector3(1.0, -1.0, 1.0),
            // Top
            cyclone::Vector3(-1.0, 1.0, 1.0),
            cyclone::Vector3(1.0, 1.0, 1.0),
            cyclone::Vector3(1.0, 1.0, -1.0),
            cyclone::Vector3(-1.0, 1.0, -1.0)
    };

    double size = 5.0; //size of cube

    std::array<cyclone::Vector3, 24> slope = {
            // Right
            cyclone::Vector3(-size, -1.0, 1.0),
            cyclone::Vector3(size, -1.0, 1.0),
            cyclone::Vector3(size, -1.0, 1.0), //face[2] 1
            cyclone::Vector3(-size, 1.0, 1.0), //face[3] 2
            // Front
            cyclone::Vector3(size, -1.0, 1.0),
            cyclone::Vector3(size, -1.0, -1.0),
            cyclone::Vector3(size, -1.0, -1.0), //face[6] 4
            cyclone::Vector3(size, -1.0, 1.0), //face[7] 3
            // Left
            cyclone::Vector3(-size, -1.0, -1.0),
            cyclone::Vector3(-size, 1.0, -1.0),
            cyclone::Vector3(size, -1.0, -1.0),
            cyclone::Vector3(size, -1.0, -1.0),
            // Back
            cyclone::Vector3(-size, -1.0, 1.0),
            cyclone::Vector3(-size, 1.0, 1.0),
            cyclone::Vector3(-size, 1.0, -1.0),
            cyclone::Vector3(-size, -1.0, -1.0),
            // Bottom
            cyclone::Vector3(-size, -1.0, 1.0),
            cyclone::Vector3(-size, -1.0, -1.0),
            cyclone::Vector3(size, -1.0, -1.0),
            cyclone::Vector3(size, -1.0, 1.0),
            // Top
            cyclone::Vector3(-size, 1.0, 1.0),
            cyclone::Vector3(size, -1.0, 1.0),
            cyclone::Vector3(size, -1.0, -1.0),
            cyclone::Vector3(-size, 1.0, -1.0),

    };

    //floor
    _obstacles.push_back(std::make_unique<Obstacle>(
            cube,
            cyclone::Vector3(0, 5, 0),
            cyclone::Vector3(0, 0, 0),
            cyclone::Vector3(100, 5, 100),
            "assets/brick.jpg")
    );
    // Left wall
        _obstacles.push_back(std::make_unique<Obstacle>(
                cube,
                cyclone::Vector3(-100, 10, 0), // Adjust position as needed
                cyclone::Vector3(0, 0, 0),
                cyclone::Vector3(5, 50, 100), // Adjust size as needed
                "assets/brick.jpg")
        );

    // Right wall
        _obstacles.push_back(std::make_unique<Obstacle>(
                cube,
                cyclone::Vector3(100, 10, 0), // Adjust position as needed
                cyclone::Vector3(0, 0, 0),
                cyclone::Vector3(5, 50, 100), // Adjust size as needed
                "assets/brick.jpg")
        );

    // Front wall
        _obstacles.push_back(std::make_unique<Obstacle>(
                cube,
                cyclone::Vector3(0, 10, 100), // Adjust position as needed
                cyclone::Vector3(0, 0, 0),
                cyclone::Vector3(100, 50, 5), // Adjust size as needed
                "assets/brick.jpg")
        );

    // Back wall
        _obstacles.push_back(std::make_unique<Obstacle>(
                cube,
                cyclone::Vector3(0, 10, -100), // Adjust position as needed
                cyclone::Vector3(0, 0, 0),
                cyclone::Vector3(100, 50, 5), // Adjust size as needed
                "assets/brick.jpg")
        );

        _obstacles.push_back(std::make_unique<Obstacle>(
                slope,
                cyclone::Vector3(0, 10, 0), // Adjust position as needed
                cyclone::Vector3(0, 0, 0),
                cyclone::Vector3(10, 10, 10), // Adjust size as needed
                "assets/brick.jpg")
        );

    auto *reg = new cyclone::World::ContactGenRegistration();
    auto temp = reg;
    for (auto &obstacle : _obstacles) {
        obstacle->init(_ball);
        if (temp->gen != nullptr && temp->next == nullptr) {
           temp->next = new cyclone::World::ContactGenRegistration();
           temp = temp->next;
        }
        temp->gen = obstacle.get();
        temp->next = nullptr;
    }
    _world->firstContactGen = reg;
}

void Window::draw() {
    glViewport(0, 0, _width, _height);
    glEnable(GL_DEPTH_TEST); // enable depth testing
    glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"

    glm::vec3 eye = _viewer->getViewPoint();
    glm::vec3 look = _viewer->getViewCenter();
    glm::vec3 up = _viewer->getUpVector();
    glm::mat4 view = lookAt(eye, look, up);
    glm::mat4 projection = perspective(45.0f, 1.0f * (float) _width / (float) _height, 0.1f, 500.0f);
    if (_skybox)
        _skybox->draw(view, projection);
    if (_ball)
        _ball->draw(view, projection, _light);
    for (auto &obstacle : _obstacles)
        obstacle->draw(view, projection, _light);
}

void Window::update() {
    auto now = std::chrono::steady_clock::now();
    std::chrono::duration<float> duration = now - _last_frame;
    _last_frame = now;
    float deltaTime = duration.count();
    mouseDragging();
    if (!run) return;
    _world->startFrame();
    if (_should_push_ball) {
        _ball->pushBall(_viewer->getViewDir());
        _should_push_ball = false;
    }
    _ball->update(deltaTime);
    _world->runPhysics(deltaTime);
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
    if (key == GLFW_KEY_R && action == GLFW_PRESS)
        run = !run;
    if (key == GLFW_KEY_G && action == GLFW_RELEASE) {
        _should_push_ball = true;
    }
}

void Window::updateCursor(GLFWwindow* glWindow, double xPos, double yPos) {
    std::unique_ptr<Window> &window = getInstance();
    window->setCursorPos(static_cast<float>(xPos), static_cast<float>(yPos));
}

void Window::mouseEvents(GLFWwindow* glWindow, int button, int action, int mods) {
    std::unique_ptr<Window> &window = getInstance();
    if (action == GLFW_PRESS) {
        double xPos, yPos;
        glfwGetCursorPos(glWindow, &xPos, &yPos);
        window->setLastCursorPos(static_cast<float>(xPos), static_cast<float>(yPos));
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
        float fraction_change_x = static_cast<float>(cursor_pos.x - last_cursor_pos.x) / static_cast<float>(width);
        float fraction_change_y = static_cast<float>(last_cursor_pos.y - cursor_pos.y) / static_cast<float>(height);
        viewer->rotate(fraction_change_x, fraction_change_y);
    }
    else if (window->getMiddleButtonDown()) {
        float fraction_change_x = (cursor_pos.x - last_cursor_pos.x) / static_cast<float>(width);
        float fraction_change_y = (last_cursor_pos.y - cursor_pos.y) / static_cast<float>(height);
        viewer->zoom(fraction_change_y);
    }
    else if (window->getRightButtonDown()) {
        float fraction_change_x = (cursor_pos.x - last_cursor_pos.x) / static_cast<float>(width);
        float fraction_change_y = (last_cursor_pos.y - cursor_pos.y) / static_cast<float>(height);
        viewer->translate(-fraction_change_x, -fraction_change_y, true);
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

Window::~Window() {
    while (_world->firstBody) {
        auto *next = _world->firstBody->next;
        delete _world->firstBody;
        _world->firstBody = next;
    }
    while (_world->firstContactGen) {
        auto *next = _world->firstContactGen->next;
        delete _world->firstContactGen;
        _world->firstContactGen = next;
    }
}
