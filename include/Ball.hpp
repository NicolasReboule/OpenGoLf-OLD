//
// Created by nicol on 03/06/2024.
//

#ifndef OPENGOLF_BALL_HPP
#define OPENGOLF_BALL_HPP

#include "particle.h"
#include "pfgen.h"
#include "GL/gl3w.h"
#include "GLFW/glfw3.h"
#include "Sphere.hpp"
#include "Light.hpp"
#include "collide_fine.h"

//TODO: Consider inheriting from Sphere
class Ball : public IDrawable, public cyclone::CollisionSphere {
public:
    Ball(float radius, cyclone::Vector3 position, cyclone::Vector3 rotation, float scale, const std::string &texturePath);

    ~Ball() override;

    void update(float duration) override;

    void draw(glm::mat4 view, glm::mat4 projection, Light light) const override;

    inline void setRadius(float radius);

    [[nodiscard]] inline float getRadius() const { return _sphere->getRadius(); }

    inline void setPosition(const cyclone::Vector3 &position) { body->setPosition(position);  _sphere->setPosition(position); }

    [[nodiscard]] inline cyclone::Vector3 getPosition() const { return body->getPosition(); }

    inline void setRotation(const cyclone::Vector3 &rotation) { _sphere->setRotation(rotation); }

    [[nodiscard]] inline cyclone::Vector3 getRotation() const { return _sphere->getRotation(); }

    inline void setScale(const float &scale) { _sphere->setScale(scale); }

    [[nodiscard]] inline float getScale() const { return _sphere->getScale(); }

    inline void setVelocity(const cyclone::Vector3 &velocity) { body->setVelocity(velocity); }

    [[nodiscard]] inline cyclone::Vector3 getVelocity() const { return body->getVelocity(); }

    inline void setAcceleration(const cyclone::Vector3 &acceleration) { body->setAcceleration(acceleration); }

    [[nodiscard]] inline cyclone::Vector3 getAcceleration() const { return body->getAcceleration(); }

    inline void setDamping(cyclone::real linear_damping, cyclone::real angular_damping) { body->setDamping(linear_damping, angular_damping); }

    inline void setLinearDamping(cyclone::real linear_damping) { body->setLinearDamping(linear_damping); }

    [[nodiscard]] inline cyclone::real getLinearDamping() const { return body->getLinearDamping(); }

    inline void setAngularDamping(cyclone::real angular_damping) { body->setAngularDamping(angular_damping); }

    [[nodiscard]] inline cyclone::real getAngularDamping() const { return body->getAngularDamping(); }

    inline void setMass(cyclone::real mass) { body->setMass(mass); }

    [[nodiscard]] inline cyclone::real getMass() const { return body->getMass(); }

    void pushBall(const glm::vec3 &direction);
private:
    std::unique_ptr<Sphere> _sphere;
};


#endif //OPENGOLF_BALL_HPP
