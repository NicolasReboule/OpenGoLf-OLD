//
// Created by nicol on 26/05/2024.
//

#ifndef OpenGoLf_WINDOW_HPP
#define OpenGoLf_WINDOW_HPP

#include <chrono>
#include "utils/Viewer.hpp"
#include "utils/Loader.h"
#include "GLFW/glfw3.h"
#include "SkyBox.hpp"
#include "IDrawables.hpp"
#include "Light.hpp"
#include "Ball.hpp"
#include "Hexahedron.hpp"
#include "world.h"
#include "Obstacle.hpp"

constexpr float DEFAULT_VIEW_POINT[3] = { 5, 5, 5 };
constexpr float DEFAULT_VIEW_CENTER[3] = { 0, 0, 0 };
constexpr float DEFAULT_UP_VECTOR[3] = { 0, 1, 0 };

class Window {
public:
    static std::unique_ptr<Window> &getInstance(int w = 800, int h = 600);

    ~Window();

    Window(const Window &) = delete;

    Window &operator=(const Window &) = delete;

    void draw();

    void update();

    static glm::mat4 lookAt(glm::vec3 eye, glm::vec3 center, glm::vec3 up);

    static glm::mat4 perspective(float fovY, float aspect, float nearZ, float farZ);

    static void resize(GLFWwindow *window, int width, int height);

    static void keyEvents(GLFWwindow *window, int key, int scancode, int action, int mods);

    static void updateCursor(GLFWwindow *window, double x_pos, double y_pos);

    static void mouseEvents(GLFWwindow *window, int button, int action, int mods);

    static void mouseDragging();

    void setCursorPos(float x, float y);

    [[nodiscard]] inline glm::vec2 getCursorPos() const { return _cursor_pos; }

    void setLastCursorPos(float x, float y);

    [[nodiscard]] inline glm::vec2 getLastCursorPos() const { return _last_cursor_pos; }

    inline void setLeftButtonDown(bool down) { _left_button_down = down; }

    [[nodiscard]] inline bool getLeftButtonDown() const { return _left_button_down; }

    inline void setRightButtonDown(bool down) { _right_button_down = down; }

    [[nodiscard]] inline bool getRightButtonDown() const { return _right_button_down; }

    inline void setMiddleButtonDown(bool down) { _middle_button_down = down; }

    [[nodiscard]] inline bool getMiddleButtonDown() const { return _middle_button_down; }

    [[nodiscard]] inline int getWidth() const { return _width; }

    [[nodiscard]] inline int getHeight() const { return _height; }

    inline void setLight(const Light &light) { _light = light; }

    [[nodiscard]] inline Light &getLight() { return _light; }

    [[nodiscard]] inline std::unique_ptr<Viewer> &getViewer() { return _viewer; }

private:
    Window(int w, int h);

    void setSize(int w, int h);

    void setAspect(float aspect);

    void init();
private:
    static std::unique_ptr<Window> _instance;

    static bool _should_push_ball;

    std::unique_ptr<Viewer> _viewer;

    std::shared_ptr<Ball> _ball;

    std::vector<std::unique_ptr<Obstacle>> _obstacles;

    std::unique_ptr<SkyBox> _skybox;

    std::unique_ptr<cyclone::World> _world;

    std::shared_ptr<cyclone::ParticleGravity> _gravity;

    int _max_contacts = 0;

    int _width;

    int _height;

    bool _left_button_down = false, _right_button_down = false, _middle_button_down = false;

    glm::vec2 _cursor_pos = glm::vec2(0, 0);

    glm::vec2 _last_cursor_pos = glm::vec2(0, 0);

    Light _light;

    std::chrono::steady_clock::time_point _last_frame;

    bool static run;
};

#endif //OpenGoLf_WINDOW_HPP
