//
// Created by nicol on 05/06/2024.
//

#ifndef OPENGOLF_OBSTACLE_HPP
#define OPENGOLF_OBSTACLE_HPP

#include <vector>
#include <array>
#include <algorithm>
#include "contacts.h"
#include "Hexahedron.hpp"
#include "Ball.hpp"

class Obstacle : public cyclone::ContactGenerator {
public:
    Obstacle(const std::array<cyclone::Vector3, 24> &vertices, cyclone::Vector3 position, cyclone::Vector3 rotation, cyclone::Vector3 scale, const std::string &texturePath);

    void init(const std::weak_ptr<Ball> &ball);

    unsigned addContact(cyclone::Contact *contact, unsigned limit) const final;

    void draw(glm::mat4 view, glm::mat4 projection, Light light) const;
private:
    enum Faces {
        FRONT = 0,
        RIGHT,
        BACK,
        LEFT,
        BOTTOM,
        TOP
    };

    std::unique_ptr<Hexahedron> _hexahedron;

    std::vector<std::weak_ptr<Ball>> _balls;

    ShaderProgram *_shaderDebug;

    float _restitution = 1.0f; //TODO: Consider making it a parameter

    float _friction = 0.0f; //TODO: Consider making it a parameter

    void drawCollider(glm::mat4 view, glm::mat4 projection) const;

    int frontContact(cyclone::Contact *contact, cyclone::Vector3 p1, cyclone::Vector3 p2, cyclone::Vector3 p3, cyclone::Vector3 p4, const std::shared_ptr<Ball> &ball) const;

    int backContact(cyclone::Contact *contact, cyclone::Vector3 p1, cyclone::Vector3 p2, cyclone::Vector3 p3, cyclone::Vector3 p4, const std::shared_ptr<Ball> &ball) const;

    int rightContact(cyclone::Contact *contact, cyclone::Vector3 p1, cyclone::Vector3 p2, cyclone::Vector3 p3, cyclone::Vector3 p4, const std::shared_ptr<Ball> &ball) const;

    int leftContact(cyclone::Contact *contact, cyclone::Vector3 p1, cyclone::Vector3 p2, cyclone::Vector3 p3, cyclone::Vector3 p4, const std::shared_ptr<Ball> &ball) const;

    int bottomContact(cyclone::Contact *contact, cyclone::Vector3 p1, cyclone::Vector3 p2, cyclone::Vector3 p3, cyclone::Vector3 p4, const std::shared_ptr<Ball> &ball) const;

    int topContact(cyclone::Contact *contact, cyclone::Vector3 p1, cyclone::Vector3 p2, cyclone::Vector3 p3, cyclone::Vector3 p4, const std::shared_ptr<Ball> &ball) const;
};

#endif //OPENGOLF_OBSTACLE_HPP
