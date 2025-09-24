//
// Created by nicol on 05/06/2024.
//

#ifndef OPENGOLF_HEXAHEDRON_HPP
#define OPENGOLF_HEXAHEDRON_HPP

#include <vector>
#include <string>
#include <iostream>
#include <array>
#include <memory>
#include "GL/gl3w.h"
#include "glm/gtc/constants.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Loader.h"
#include "core.h"
#include "Light.hpp"
#include "IDrawables.hpp"

//TODO: Refactor to an hexahedron with custom vertices (Need to be able to do slides) but by default cube
class Hexahedron {
public:
    Hexahedron(std::array<cyclone::Vector3, 24> vertices, const std::string &texturePath);

    void draw(glm::mat4 view, glm::mat4 projection, Light light) const;

    void setTexture(const std::string &texturePath);

    [[nodiscard]] inline const std::string &getTexturePath() const { return _texture_path; }

    inline void setPosition(const cyclone::Vector3 &position) { _position = position; }

    [[nodiscard]] inline cyclone::Vector3 getPosition() const { return _position; }

    inline void setRotation(const cyclone::Vector3 &rotation) { _rotation = rotation; }

    [[nodiscard]] inline cyclone::Vector3 getRotation() const { return _rotation; }

    inline void setScale(const cyclone::Vector3 &scale) { _scale = scale; }

    [[nodiscard]] inline cyclone::Vector3 getScale() const { return _scale; }

    void setVertices(const std::array<cyclone::Vector3, 24> &vertices) { _vertices = vertices; }

    [[nodiscard]] inline std::array<cyclone::Vector3, 24> getVertices() const { return _vertices; }

    [[nodiscard]] inline std::array<cyclone::Vector3, 24> getVertices() { return _vertices; }

private:
    void setup();

    void initShader();

    void generate(std::array<float, 24 * 3> &normals, std::array<std::pair<float, float>, 24> &tex_coords);
private:
    GLuint _vao = 0, _ibo = 0, _texture = 0;

    GLuint _vbo_position = 0, _vbo_normal = 0, _vbo_tex_coord = 0;

    std::string _texture_path;

    cyclone::Vector3 _position = cyclone::Vector3(0, 0, 0);

    cyclone::Vector3 _rotation = cyclone::Vector3(0, 0, 0);

    cyclone::Vector3 _scale = cyclone::Vector3(1, 1, 1);

    std::unique_ptr<ShaderProgram> _shader;

    std::array<cyclone::Vector3, 24> _vertices;
};


#endif //OPENGOLF_HEXAHEDRON_HPP
