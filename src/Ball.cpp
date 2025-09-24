//
// Created by nicol on 03/06/2024.
//

#include "Ball.hpp"

Ball::Ball(float radius, cyclone::Vector3 position, cyclone::Vector3 rotation, float scale, const std::string &texture_path) {
    body = new cyclone::RigidBody();
    body->setAwake(true);
    body->setPosition(position);
    body->setRotation(rotation);
    body->setAcceleration(cyclone::Vector3::GRAVITY);
    //TODO: Find right damping and mass
    body->setDamping(0.8, 0.8);
    body->setMass(1.0);
    _sphere = std::make_unique<Sphere>(radius, 50, 50, texture_path);
    _sphere->setPosition(position);
    _sphere->setRotation(rotation);
    _sphere->setScale(scale);
    this->radius = radius * scale;
}

void Ball::update(float duration) {
    _sphere->setPosition(body->getPosition());
}

void Ball::draw(glm::mat4 view, glm::mat4 projection, Light light) const {
    _sphere->draw(view, projection, light);
}

void Ball::setRadius(float radius) {
    _sphere->setRadius(radius);
    this->radius = radius * _sphere->getScale();
}

Ball::~Ball() {
    delete body;
}

void Ball::pushBall(const glm::vec3 &direction) {
    body->setVelocity(cyclone::Vector3(direction.x, direction.y, direction.z) * 50);
}
