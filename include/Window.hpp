//
// Created by nicol on 26/05/2024.
//

#ifndef OpenGoLf_WINDOW_HPP
#define OpenGoLf_WINDOW_HPP

#include "utils/Viewer.hpp"
#include "utils/Loader.h"
#include "GLFW/glfw3.h"

const float DEFAULT_VIEW_POINT[3] = { 5, 5, 5 };
const float DEFAULT_VIEW_CENTER[3] = { 0, 0, 0 };
const float DEFAULT_UP_VECTOR[3] = { 0, 1, 0 };

class Window {
public:
    static std::unique_ptr<Window> &getInstance(int w = 800, int h = 600);

    Window(const Window &) = delete;

    Window &operator=(const Window &) = delete;

    void draw();

    static glm::mat4 lookAt(glm::vec3 eye, glm::vec3 center, glm::vec3 up);

    static glm::mat4 perspective(float fovY, float aspect, float nearZ, float farZ);

    static void resize(GLFWwindow *window, int width, int height);

    static void keyEvents(GLFWwindow *window, int key, int scancode, int action, int mods);

    static void updateCursor(GLFWwindow *window, double x_pos, double y_pos);

    static void mouseEvents(GLFWwindow *window, int button, int action, int mods);

    static void mouseDragging(double width, double height);

    void setCursorPos(float x, float y);

    [[nodiscard]] inline glm::vec2 getCursorPos() const { return _cursor_pos; }

    void setLastCursorPos(float x, float y);

    [[nodiscard]] inline glm::vec2 getLastCursorPos() const { return _last_cursor_pos; }

    inline void setLeftButtonDown(bool down) { _left_button_down = down; }

    inline void setRightButtonDown(bool down) { _right_button_down = down; }

    inline void setMiddleButtonDown(bool down) { _middle_button_down = down; }

    [[nodiscard]] inline bool getLeftButtonDown() const { return _left_button_down; }

    [[nodiscard]] inline bool getRightButtonDown() const { return _right_button_down; }

    [[nodiscard]] inline bool getMiddleButtonDown() const { return _middle_button_down; }

    [[nodiscard]] inline int getWidth() const { return _width; }

    [[nodiscard]] inline int getHeight() const { return _height; }

    [[nodiscard]] inline std::unique_ptr<Viewer> &getViewer() { return _viewer; }

private:
    static std::unique_ptr<Window> _instance;

    std::unique_ptr<Viewer> _viewer;

    int _width;
    int _height;

    bool _left_button_down = false, _right_button_down = false, _middle_button_down = false;
    glm::vec2 _cursor_pos = glm::vec2(0, 0);
    glm::vec2 _last_cursor_pos = glm::vec2(0, 0);

    Window(int w, int h);

    void setSize(int w, int h);

    void setAspect(float aspect);
};

#endif //OpenGoLf_WINDOW_HPP
