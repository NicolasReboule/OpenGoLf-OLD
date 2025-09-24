//
// Created by nicol on 05/06/2024.
//

#include "Hexahedron.hpp"
#include "stb-master/stb_image.h"

Hexahedron::Hexahedron(std::array<cyclone::Vector3, 24> vertices, const std::string &texturePath):
    _vertices(vertices)
{
    _texture_path = texturePath;
    setup();
    initShader();
}

void Hexahedron::initShader() {
    _shader = std::make_unique<ShaderProgram>();
    _shader->initFromFiles("shaders/object/ball.vert", "shaders/object/ball.frag");
    _shader->addUniform("LightPosition");
    _shader->addUniform("LightIntensity");
    _shader->addUniform("Ka");
    _shader->addUniform("Kd");
    _shader->addUniform("Ks");
    _shader->addUniform("shiness");

    _shader->addUniform("ModelViewMatrix"); // View*Model : mat4
    _shader->addUniform("NormalMatrix"); // Refer next slide : mat3
    _shader->addUniform("MVP"); // Projection * View * Model : mat4
    _shader->addUniform("Tex");
}

void Hexahedron::draw(glm::mat4 view, glm::mat4 projection, Light light) const {
    _shader->use();
    glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(_position.x, _position.y, _position.z));
    glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), glm::radians(static_cast<float>(_rotation.x)), glm::vec3(1.0f, 0.0f, 0.0f));
    rotate *= glm::rotate(glm::mat4(1.0f), glm::radians(static_cast<float>(_rotation.y)), glm::vec3(0.0f, 1.0f, 0.0f));
    rotate *= glm::rotate(glm::mat4(1.0f), glm::radians(static_cast<float>(_rotation.z)), glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(_scale.x, _scale.y, _scale.z));
    glm::mat4 model = translate * rotate * scale; // Combination of transformation matrix

    glm::mat4 model_view = view * model;
    glm::mat4 mvp = projection * view * model;

    glm::mat4 inverse_mvp = glm::inverse(model_view);
    glm::mat3 normal_matrix = glm::mat3(glm::transpose(inverse_mvp)); //normal matrix

    glm::vec3 Ka(0.3, 0.3, 0.3);
    glm::vec3 Kd(1, 1, 1);
    glm::vec3 Ks(0.3, 0.3, 0.3);
    float shiness = 10;

    glUniform4fv(_shader->uniform("LightPosition"), 1, glm::value_ptr(light.position));
    glUniform3fv(_shader->uniform("LightIntensity"), 1, glm::value_ptr(light.intensity));

    glUniform3fv(_shader->uniform("Ka"), 1, glm::value_ptr(Ka));
    glUniform3fv(_shader->uniform("Kd"), 1, glm::value_ptr(Kd));
    glUniform3fv(_shader->uniform("Ks"), 1, glm::value_ptr(Ks));
    glUniform1fv(_shader->uniform("shiness"), 1, &shiness);

    glUniformMatrix4fv(_shader->uniform("ModelViewMatrix"), 1, GL_FALSE, glm::value_ptr(model_view));
    glUniformMatrix3fv(_shader->uniform("NormalMatrix"), 1, GL_FALSE, glm::value_ptr(normal_matrix));
    glUniformMatrix4fv(_shader->uniform("MVP"), 1, GL_FALSE, glm::value_ptr(mvp));

    glActiveTexture(GL_TEXTURE0);
    glUniform1i(_shader->uniform("Tex"), 0);

    if (_vao) {
        glBindTexture(GL_TEXTURE_2D, _texture);
        glBindVertexArray(_vao);
        GLint size;
        glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
        glDrawElements(GL_TRIANGLES, size / sizeof(GLuint), GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    _shader->disable();
}

void Hexahedron::setup() {
    GLuint elements[] = {
            0,2,1, 0,3,2, 4,6,5, 4,7,6,
            8,10,9, 8,11,10, 12,14,13, 12,15,14,
            16,18,17, 16,19,18, 20,22,21, 20,23,22
    };

    std::array<float, 24 * 3> normals;
    std::array<std::pair<float, float>, 24> tex_coords;
    std::array<float, 24 * 3> flat_vertices;
    for (int i = 0; i < 24; i++) {
        flat_vertices[i * 3] = static_cast<float>(_vertices[i].x);
        flat_vertices[i * 3 + 1] = static_cast<float>(_vertices[i].y);
        flat_vertices[i * 3 + 2] = static_cast<float>(_vertices[i].z);
    }

    generate(normals, tex_coords);
    //create vao
    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);

    glGenBuffers(1, &_vbo_position);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_position);
    glBufferData(GL_ARRAY_BUFFER,  sizeof(flat_vertices), flat_vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, //attribute
                          3, //number of elements per vertex, here (x,y,z)
                          GL_FLOAT, //type of each element
                          GL_FALSE, //take our values as-is
                          0, //stride
                          nullptr //array buffer offset
    );
    glEnableVertexAttribArray(0);
    glGenBuffers(1, &_vbo_normal);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_normal);
    glBufferData(GL_ARRAY_BUFFER,  normals.size() * sizeof(float) * 3, &normals, GL_STATIC_DRAW);
    glVertexAttribPointer(1, //attribute
                          3, //number of elements per vertex, here (r,g,b)
                          GL_FLOAT, //type of each element
                          GL_FALSE, //take our values as-is
                          0, //stride
                          nullptr // starts at position 4 of the array
    );
    glEnableVertexAttribArray(1);

    glGenTextures(1, &_texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _texture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    int width, height, n_channels;
    unsigned char* data = stbi_load(_texture_path.c_str(), &width, &height, &n_channels, 0);
    if (data == nullptr) {
        std::cerr << "Error loading texture " << _texture_path << std::endl;
        exit(1);
    }
    auto format = n_channels == 3 ? GL_RGB : GL_RGBA;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenBuffers(1, &_vbo_tex_coord);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_tex_coord);
    glBufferData(GL_ARRAY_BUFFER, 2 * tex_coords.size() * sizeof(float), &tex_coords, GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(2);

    glGenBuffers(1, &_ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

    glBindVertexArray(0); //close current vao
}

void Hexahedron::setTexture(const std::string &texturePath) {
    _texture_path = texturePath;
    setup();
}

void Hexahedron::generate(std::array<float, 24 * 3> &normals, std::array<std::pair<float, float>, 24> &tex_coords) {
    for (int i = 0; i < 24; i += 4) {
        // Calculate two vectors that lie on the face
        cyclone::Vector3 v1 = _vertices[i + 1] - _vertices[i];
        cyclone::Vector3 v2 = _vertices[i + 2] - _vertices[i];

        // Calculate the normal of the face
        cyclone::Vector3 normal = v1.cross(v2);

        // Normalize the normal
        normal = normal.unit();

        // Assign the normal to each vertex of the face
        for (int j = 0; j < 4; j++) {
            normals[(i + j) * 3] = normal.x;
            normals[(i + j) * 3 + 1] = normal.y;
            normals[(i + j) * 3 + 2] = normal.z;
        }

        // Assign texture coordinates to each vertex of the face
        tex_coords[i] = std::make_pair(0.0f, 0.0f);
        tex_coords[i + 1] = std::make_pair(1.0f, 0.0f);
        tex_coords[i + 2] = std::make_pair(1.0f, 1.0f);
        tex_coords[i + 3] = std::make_pair(0.0f, 1.0f);
    }
}
