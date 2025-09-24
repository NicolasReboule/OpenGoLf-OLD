//
// Created by nicol on 03/06/2024.
//

#ifndef OPENGOLF_SPHERE_HPP
#define OPENGOLF_SPHERE_HPP

#include <vector>
#include <string>
#include <iostream>
#include <memory>
#include "GL/gl3w.h"
#include "glm/gtc/constants.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Loader.h"
#include "core.h"
#include "Light.hpp"
#include "IDrawables.hpp"

class Sphere {
public:
    Sphere(float radius, GLuint sl, GLuint st, const std::string &texturePath);

    void draw(glm::mat4 view, glm::mat4 projection, Light light) const;

    void setRadius(float radius);

    [[nodiscard]] inline float getRadius() const { return _radius; }

    inline void setPosition(const cyclone::Vector3 &position) { _position = position; }

    [[nodiscard]] inline cyclone::Vector3 getPosition() const { return _position; }

    inline void setRotation(const cyclone::Vector3 &rotation) { _rotation = rotation; }

    [[nodiscard]] inline cyclone::Vector3 getRotation() const { return _rotation; }

    inline void setScale(const float &scale) { _scale = scale; }

    [[nodiscard]] inline float getScale() const { return _scale; }
private:
    void setup();

    void generateVertices(float *, float *, float *, GLuint *) const;

    void initShader();
private:
    GLuint _vao = 0, _ibo = 0, _texture = 0;

    GLuint _vbo_position = 0, _vbo_normal = 0, _vbo_tex_coord = 0;

    GLuint _n_vertices = 0, _n_elements = 0;

    float _radius;

    GLuint _slices, _stacks;

    std::string _texture_path;

    cyclone::Vector3 _position = cyclone::Vector3(0, 0, 0);

    cyclone::Vector3 _rotation = cyclone::Vector3(0, 0, 0);

    float _scale = 1.0;

    std::unique_ptr<ShaderProgram> _shader;
};


#endif //OPENGOLF_SPHERE_HPP
