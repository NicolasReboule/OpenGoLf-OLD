//
// Created by nicol on 05/06/2024.
//

#include "Obstacle.hpp"

Obstacle::Obstacle(const std::array<cyclone::Vector3, 24> &vertices, cyclone::Vector3 position, cyclone::Vector3 rotation, cyclone::Vector3 scale, const std::string &texturePath)
{
    _hexahedron = std::make_unique<Hexahedron>(vertices, texturePath);
    _hexahedron->setPosition(position);
    _hexahedron->setRotation(rotation);
    _hexahedron->setScale(scale);

    _shaderDebug = new ShaderProgram();
    // TODO: Create a shader manager to avoid multiple loading of the same shader
    _shaderDebug->initFromFiles("shaders/debug/simple.vert", "shaders/debug/simple.frag");
    _shaderDebug->addUniform("MVP");
}

void Obstacle::init(const std::weak_ptr<Ball> &ball) {
    _balls.push_back(ball);
}

struct ContactPoints {
    cyclone::Vector3 minX;
    cyclone::Vector3 maxX;
    cyclone::Vector3 minZ;
    cyclone::Vector3 maxZ;
    cyclone::Vector3 minY;
    cyclone::Vector3 maxY;

    ContactPoints(cyclone::Vector3 p1, cyclone::Vector3 p2, cyclone::Vector3 p3, cyclone::Vector3 p4) {
        std::array<cyclone::Vector3, 4> points = {p1, p2, p3, p4};

        minX = *std::min_element(points.begin(), points.end(), [](const cyclone::Vector3 &a, const cyclone::Vector3 &b) {
            return a.x < b.x;
        });
        maxX = *std::max_element(points.begin(), points.end(), [](const cyclone::Vector3 &a, const cyclone::Vector3 &b) {
            return a.x < b.x;
        });
        minY = *std::min_element(points.begin(), points.end(), [](const cyclone::Vector3 &a, const cyclone::Vector3 &b) {
            return a.y < b.y;
        });
        maxY = *std::max_element(points.begin(), points.end(), [](const cyclone::Vector3 &a, const cyclone::Vector3 &b) {
            return a.y < b.y;
        });
        minZ = *std::min_element(points.begin(), points.end(), [](const cyclone::Vector3 &a, const cyclone::Vector3 &b) {
            return a.z < b.z;
        });
        maxZ = *std::max_element(points.begin(), points.end(), [](const cyclone::Vector3 &a, const cyclone::Vector3 &b) {
            return a.z < b.z;
        });
    }
};

unsigned Obstacle::addContact(cyclone::Contact *contact, unsigned limit) const
{
    unsigned count = 0;
    auto vertices = _hexahedron->getVertices();
    int face_index = 0;
    std::array<std::array<int, 4>, 6> faces = {{
        {0, 1, 2, 3}, // Face Front
        {4, 5, 6, 7}, // Face Right
        {8, 9, 10, 11}, // Face Back
        {12, 13, 14, 15}, // Face Left
        {16, 17, 18, 19}, // Face Bottom
        {20, 21, 22, 23}  // Face Top
    }};
    for (auto &ball_weak : _balls) {
        if (ball_weak.expired()) continue;
        auto ball = ball_weak.lock();
        for (auto &face: faces) {
            cyclone::Vector3 p1 = cyclone::Vector3(
                    vertices[face[1]].x,
                    vertices[face[1]].y,
                    vertices[face[1]].z);
            cyclone::Vector3 p2 = cyclone::Vector3(
                    vertices[face[2]].x,
                    vertices[face[2]].y,
                    vertices[face[2]].z);
            cyclone::Vector3 p3 = cyclone::Vector3(
                    vertices[face[0]].x,
                    vertices[face[0]].y,
                    vertices[face[0]].z);
            cyclone::Vector3 p4 = cyclone::Vector3(
                    vertices[face[3]].x,
                    vertices[face[3]].y,
                    vertices[face[3]].z);

            auto p1_glm = glm::vec3(p1.x, p1.y, p1.z);
            auto p2_glm = glm::vec3(p2.x, p2.y, p2.z);
            auto p3_glm = glm::vec3(p3.x, p3.y, p3.z);
            auto p4_glm = glm::vec3(p4.x, p4.y, p4.z);
            float rotation_x = _hexahedron->getRotation().x;
            float rotation_y = _hexahedron->getRotation().y;
            float rotation_z = _hexahedron->getRotation().z;
            glm::mat4 T = glm::translate(glm::mat4(1.0f),glm::vec3(_hexahedron->getPosition().x, _hexahedron->getPosition().y, _hexahedron->getPosition().z));
            glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(_hexahedron->getScale().x, _hexahedron->getScale().y, _hexahedron->getScale().z));
            glm::mat4 R = glm::rotate(glm::mat4(1.0f), glm::radians(rotation_x), glm::vec3(1.0f, 0.0f, 0.0f));
            R *= glm::rotate(glm::mat4(1.0f), glm::radians(rotation_y), glm::vec3(0.0f, 1.0f, 0.0f));
            R *= glm::rotate(glm::mat4(1.0f), glm::radians(rotation_z), glm::vec3(0.0f, 0.0f, 1.0f));

            p1_glm = glm::vec3(S * glm::vec4(p1_glm, 1.0f));
            p2_glm = glm::vec3(S * glm::vec4(p2_glm, 1.0f));
            p3_glm = glm::vec3(S * glm::vec4(p3_glm, 1.0f));
            p4_glm = glm::vec3(S * glm::vec4(p4_glm, 1.0f));

            p1_glm = glm::vec3(R * glm::vec4(p1_glm, 1.0f));
            p2_glm = glm::vec3(R * glm::vec4(p2_glm, 1.0f));
            p3_glm = glm::vec3(R * glm::vec4(p3_glm, 1.0f));
            p4_glm = glm::vec3(R * glm::vec4(p4_glm, 1.0f));

            p1_glm = glm::vec3(T * glm::vec4(p1_glm, 1.0f));
            p2_glm = glm::vec3(T * glm::vec4(p2_glm, 1.0f));
            p3_glm = glm::vec3(T * glm::vec4(p3_glm, 1.0f));
            p4_glm = glm::vec3(T * glm::vec4(p4_glm, 1.0f));

            p1 = cyclone::Vector3(p1_glm.x, p1_glm.y, p1_glm.z);
            p2 = cyclone::Vector3(p2_glm.x, p2_glm.y, p2_glm.z);
            p3 = cyclone::Vector3(p3_glm.x, p3_glm.y, p3_glm.z);
            p4 = cyclone::Vector3(p4_glm.x, p4_glm.y, p4_glm.z);

            switch (face_index) {
                case Faces::FRONT:
                    count += frontContact(contact, p1, p2, p3, p4, ball);
                    break;
                case Faces::RIGHT:
                    count += rightContact(contact, p1, p2, p3, p4, ball);
                    break;
                case Faces::BACK:
                    count += backContact(contact, p1, p2, p3, p4, ball);
                    break;
                case Faces::LEFT:
                    count += leftContact(contact, p1, p2, p3, p4, ball);
                    break;
                case Faces::BOTTOM:
                    count += bottomContact(contact, p1, p2, p3, p4, ball);
                    break;
                case Faces::TOP:
                    count += topContact(contact, p1, p2, p3, p4, ball);
                    break;
                default:
                    break;
            }
            face_index++;
            if (count >= limit)
                return count;
        }
    }
    return count;
}

int Obstacle::frontContact(cyclone::Contact *contact, cyclone::Vector3 p1, cyclone::Vector3 p2, cyclone::Vector3 p3,
                           cyclone::Vector3 p4, const std::shared_ptr<Ball> &ball) const {
    int count = 0;
    cyclone::Vector3 position = ball->getPosition();
    cyclone::Vector3 v1 = p2 - p1;
    cyclone::Vector3 v2 = p3 - p1;
    v1.normalise();
    v2.normalise();
    cyclone::Vector3 normal = v1.cross(v2);
    cyclone::Vector3 plane_ball = position - p1;
    double distance = plane_ball.dot(normal);
    float size = ball->getRadius();
    ContactPoints cp(p1, p2, p3, p4);
    if (distance > 0 && distance < size && position.x >= cp.minX.x && position.x <= cp.maxX.x && position.y >= cp.minY.y && position.y <= cp.maxY.y) {
        if ((position.z < cp.minZ.z || position.z > cp.maxZ.z) && cp.minZ.z != cp.maxZ.z) {
            return count;
        }
        contact->body[0] = ball->body;
        contact->body[1] = nullptr;
        contact->contactNormal = normal;
        contact->penetration = size - distance;
        contact->restitution = _restitution;
        contact->friction = _friction;
        contact->contactPoint = position;
        count++;
    }
    return count;
}

int Obstacle::rightContact(cyclone::Contact *contact, cyclone::Vector3 p1, cyclone::Vector3 p2, cyclone::Vector3 p3,
                          cyclone::Vector3 p4, const std::shared_ptr<Ball> &ball) const {
    int count = 0;
    cyclone::Vector3 position = ball->getPosition();
    cyclone::Vector3 v1 = p2 - p1;
    cyclone::Vector3 v2 = p3 - p1;
    v1.normalise();
    v2.normalise();
    cyclone::Vector3 normal = v1.cross(v2);
    cyclone::Vector3 plane_ball = position - p1;
    double distance = plane_ball.dot(normal);
    float size = ball->getRadius();
    ContactPoints cp(p1, p2, p3, p4);
    if (distance > 0 && distance < size  && position.y >= cp.minY.y && position.y <= cp.maxY.y && position.z >= cp.minZ.z && position.z <= cp.maxZ.z) {
        if ((position.x < cp.minX.x || position.x > cp.maxX.x) && cp.minX.x != cp.maxX.x) {
            return count;
        }
        contact->body[0] = ball->body;
        contact->body[1] = nullptr;
        contact->contactNormal = normal;
        contact->penetration = size - distance;
        contact->restitution = _restitution;
        contact->friction = _friction;
        contact->contactPoint = position;
        count++;
    }
    return count;
}

int Obstacle::backContact(cyclone::Contact *contact, cyclone::Vector3 p1, cyclone::Vector3 p2, cyclone::Vector3 p3,
                           cyclone::Vector3 p4, const std::shared_ptr<Ball> &ball) const {
    int count = 0;
    cyclone::Vector3 position = ball->getPosition();
    cyclone::Vector3 v1 = p2 - p1;
    cyclone::Vector3 v2 = p3 - p1;
    v1.normalise();
    v2.normalise();
    cyclone::Vector3 normal = v1.cross(v2);
    cyclone::Vector3 plane_ball = position - p1;
    double distance = plane_ball.dot(normal);
    float size = ball->getRadius();
    ContactPoints cp(p1, p2, p3, p4);
    if (distance > 0 && distance < size && position.x >= cp.minX.x && position.x <= cp.maxX.x && position.y >= cp.minY.y && position.y <= cp.maxY.y) {
        if ((position.z < cp.minZ.z || position.z > cp.maxZ.z) && cp.minZ.z != cp.maxZ.z) {
            return count;
        }
        contact->body[0] = ball->body;
        contact->body[1] = nullptr;
        contact->contactNormal = normal;
        contact->penetration = size - distance;
        contact->restitution = _restitution;
        contact->friction = _friction;
        contact->contactPoint = position;
        count++;
    }
    return count;
}

int Obstacle::leftContact(cyclone::Contact *contact, cyclone::Vector3 p1, cyclone::Vector3 p2, cyclone::Vector3 p3,
                          cyclone::Vector3 p4, const std::shared_ptr<Ball> &ball) const {
    int count = 0;
    cyclone::Vector3 position = ball->getPosition();
    cyclone::Vector3 v1 = p2 - p1;
    cyclone::Vector3 v2 = p3 - p1;
    v1.normalise();
    v2.normalise();
    cyclone::Vector3 normal = v1.cross(v2);
    cyclone::Vector3 plane_ball = position - p1;
    double distance = plane_ball.dot(normal);
    float size = ball->getRadius();
    ContactPoints cp(p1, p2, p3, p4);
    if (distance > 0 && distance < size && position.y >= cp.minY.y && position.y <= cp.maxY.y && position.z >= cp.minZ.z && position.z <= cp.maxZ.z) {
        if ((position.x > cp.minX.x || position.x < cp.maxX.x) && cp.minX.x != cp.maxX.x) {
            return count;
        }
        contact->body[0] = ball->body;
        contact->body[1] = nullptr;
        contact->contactNormal = normal;
        contact->penetration = size - distance;
        contact->restitution = _restitution;
        contact->friction = _friction;
        contact->contactPoint = position;
        count++;
    }
    return count;
}

int Obstacle::bottomContact(cyclone::Contact *contact, cyclone::Vector3 p1, cyclone::Vector3 p2, cyclone::Vector3 p3,
                            cyclone::Vector3 p4, const std::shared_ptr<Ball> &ball) const {
    int count = 0;
    cyclone::Vector3 position = ball->getPosition();
    cyclone::Vector3 v1 = p2 - p1;
    cyclone::Vector3 v2 = p3 - p1;
    v1.normalise();
    v2.normalise();
    cyclone::Vector3 normal = v1.cross(v2);
    cyclone::Vector3 plane_ball = position - p1;
    double distance = plane_ball.dot(normal);
    float size = ball->getRadius();
    ContactPoints cp(p1, p2, p3, p4);
    if (distance > 0 && distance < size && position.x >= cp.minX.x && position.x <= cp.maxX.x && position.z >= cp.minZ.z && position.z <= cp.maxZ.z) {
        if ((position.y > cp.minY.y || position.y < cp.maxY.y) && cp.minY.y != cp.maxY.y) {
            return count;
        }
        contact->body[0] = ball->body;
        contact->body[1] = nullptr;
        contact->contactNormal = normal;
        contact->penetration = size - distance;
        contact->restitution = _restitution;
        contact->friction = _friction;
        contact->contactPoint = position;
        count++;
    }
    return count;
}

int Obstacle::topContact(cyclone::Contact *contact, cyclone::Vector3 p1, cyclone::Vector3 p2, cyclone::Vector3 p3, cyclone::Vector3 p4, const std::shared_ptr<Ball> &ball) const {
    int count = 0;
    cyclone::Vector3 position = ball->getPosition();
    cyclone::Vector3 v1 = p2 - p1;
    cyclone::Vector3 v2 = p3 - p1;
    v1.normalise();
    v2.normalise();
    cyclone::Vector3 normal = v1.cross(v2);
    cyclone::Vector3 plane_ball = position - p1;
    double distance = plane_ball.dot(normal);
    float size = ball->getRadius();
    ContactPoints cp(p1, p2, p3, p4);
    if (distance > 0 && distance < size && position.x >= cp.minX.x && position.x <= cp.maxX.x && position.z >= cp.minZ.z && position.z <= cp.maxZ.z) {
        if ((position.y < cp.minY.y || position.y > cp.maxY.y) && cp.minY.y != cp.maxY.y) {
            return count;
        }
        contact->body[0] = ball->body;
        contact->body[1] = nullptr;
        contact->contactNormal = normal;
        contact->penetration = size - distance;
        contact->restitution = _restitution;
        contact->friction = _friction;
        contact->contactPoint = position;
        count++;
    }
    return count;
}

void Obstacle::drawCollider(glm::mat4 view, glm::mat4 projection) const {
    int face_index = 0;
    auto vertices = _hexahedron->getVertices();
    std::vector<glm::vec3> debugVertices;
    std::array<std::array<int, 4>, 6> faces = {{
                                                       {0, 1, 2, 3}, // Face Front
                                                       {4, 5, 6, 7}, // Face Right
                                                       {8, 9, 10, 11}, // Face Back
                                                       {12, 13, 14, 15}, // Face Left
                                                       {16, 17, 18, 19}, // Face Bottom
                                                       {20, 21, 22, 23}  // Face Top
                                               }};
    for (auto &face: faces) {
        cyclone::Vector3 p1 = cyclone::Vector3(
                vertices[face[1]].x,
                vertices[face[1]].y,
                vertices[face[1]].z);
        cyclone::Vector3 p2 = cyclone::Vector3(
                vertices[face[2]].x,
                vertices[face[2]].y,
                vertices[face[2]].z);
        cyclone::Vector3 p3 = cyclone::Vector3(
                vertices[face[0]].x,
                vertices[face[0]].y,
                vertices[face[0]].z);
        cyclone::Vector3 p4 = cyclone::Vector3(
                vertices[face[3]].x,
                vertices[face[3]].y,
                vertices[face[3]].z);

        auto p1_glm = glm::vec3(p1.x, p1.y, p1.z);
        auto p2_glm = glm::vec3(p2.x, p2.y, p2.z);
        auto p3_glm = glm::vec3(p3.x, p3.y, p3.z);
        auto p4_glm = glm::vec3(p4.x, p4.y, p4.z);
        float rotation_x = _hexahedron->getRotation().x;
        float rotation_y = _hexahedron->getRotation().y;
        float rotation_z = _hexahedron->getRotation().z;
        glm::mat4 T = glm::translate(glm::mat4(1.0f),
                                     glm::vec3(_hexahedron->getPosition().x, _hexahedron->getPosition().y,
                                               _hexahedron->getPosition().z));
        glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(_hexahedron->getScale().x, _hexahedron->getScale().y,
                                                            _hexahedron->getScale().z));
        glm::mat4 R = glm::rotate(glm::mat4(1.0f), glm::radians(rotation_x), glm::vec3(1.0f, 0.0f, 0.0f));
        R *= glm::rotate(glm::mat4(1.0f), glm::radians(rotation_y), glm::vec3(0.0f, 1.0f, 0.0f));
        R *= glm::rotate(glm::mat4(1.0f), glm::radians(rotation_z), glm::vec3(0.0f, 0.0f, 1.0f));

        p1_glm = glm::vec3(S * glm::vec4(p1_glm, 1.0f));
        p2_glm = glm::vec3(S * glm::vec4(p2_glm, 1.0f));
        p3_glm = glm::vec3(S * glm::vec4(p3_glm, 1.0f));
        p4_glm = glm::vec3(S * glm::vec4(p4_glm, 1.0f));

        p1_glm = glm::vec3(R * glm::vec4(p1_glm, 1.0f));
        p2_glm = glm::vec3(R * glm::vec4(p2_glm, 1.0f));
        p3_glm = glm::vec3(R * glm::vec4(p3_glm, 1.0f));
        p4_glm = glm::vec3(R * glm::vec4(p4_glm, 1.0f));

        p1_glm = glm::vec3(T * glm::vec4(p1_glm, 1.0f));
        p2_glm = glm::vec3(T * glm::vec4(p2_glm, 1.0f));
        p3_glm = glm::vec3(T * glm::vec4(p3_glm, 1.0f));
        p4_glm = glm::vec3(T * glm::vec4(p4_glm, 1.0f));

        p1 = cyclone::Vector3(p1_glm.x, p1_glm.y, p1_glm.z);
        p2 = cyclone::Vector3(p2_glm.x, p2_glm.y, p2_glm.z);
        p3 = cyclone::Vector3(p3_glm.x, p3_glm.y, p3_glm.z);
        p4 = cyclone::Vector3(p4_glm.x, p4_glm.y, p4_glm.z);

        if (debugVertices.empty() ||
            debugVertices.size() != face_index * 6 && debugVertices.size() < 6 * 6) {
            debugVertices.emplace_back(glm::vec3(p1.x, p1.y, p1.z));
            debugVertices.push_back(glm::vec3(p2.x, p2.y, p2.z));
            debugVertices.push_back(glm::vec3(p3.x, p3.y, p3.z));
            debugVertices.push_back(glm::vec3(p1.x, p1.y, p1.z));
            debugVertices.push_back(glm::vec3(p3.x, p3.y, p3.z));
            debugVertices.push_back(glm::vec3(p4.x, p4.y, p4.z));
        }
        face_index++;
    }


    static std::array<glm::vec3, 4> points_color = {
            glm::vec3(1.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f),
            glm::vec3(0.0f, 0.0f, 1.0f),
            glm::vec3(1.0f, 1.0f, 1.0f)
    };

    glm::mat4 mvp = projection * view;
    GLuint _vaoHandle;
    glGenVertexArrays(1, &_vaoHandle);
    glBindVertexArray(_vaoHandle);

    GLuint _vbos[2];
    glGenBuffers(2, _vbos);
    // Generate and bind VBO for vertices
    glGenBuffers(1, &_vbos[0]);
    glBindBuffer(GL_ARRAY_BUFFER, _vbos[0]);
    glBufferData(GL_ARRAY_BUFFER, debugVertices.size() * sizeof(glm::vec3), debugVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    // Generate and bind VBO for colors
    glGenBuffers(1, &_vbos[1]);
    glBindBuffer(GL_ARRAY_BUFFER, _vbos[1]);
    glBufferData(GL_ARRAY_BUFFER, points_color.size() * sizeof(glm::vec3), points_color.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(1);

    // Unbind VAO
    glBindVertexArray(0);

    _shaderDebug->use();
    glUniformMatrix4fv(_shaderDebug->uniform("MVP"), 1, GL_FALSE, glm::value_ptr(mvp));
    glBindVertexArray(_vaoHandle);
    glDrawArrays(GL_TRIANGLES, 0, debugVertices.size());
    glBindVertexArray(0);
    _shaderDebug->disable();
}

void Obstacle::draw(glm::mat4 view, glm::mat4 projection, Light light) const {
    _hexahedron->draw(view, projection, light);
//    drawCollider(view, projection);
}
