//
// Created by nicol on 06/06/2024.
//

#ifndef OPENGOLF_LIGHT_HPP
#define OPENGOLF_LIGHT_HPP

#include "glm/glm.hpp"

struct Light {
    glm::vec4 position;
    glm::vec3 intensity;
    glm::vec3 color;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

#endif //OPENGOLF_LIGHT_HPP
