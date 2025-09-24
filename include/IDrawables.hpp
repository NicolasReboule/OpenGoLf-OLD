//
// Created by nicol on 06/06/2024.
//

#ifndef OPENGOLF_IDRAWABLES_HPP
#define OPENGOLF_IDRAWABLES_HPP

#include "Light.hpp"

class IDrawable {
public:
    virtual ~IDrawable() = default;

    virtual void draw(glm::mat4 view, glm::mat4 projection, Light light) const = 0;

    virtual void update(float duration) = 0;
};

#endif //OPENGOLF_IDRAWABLES_HPP
