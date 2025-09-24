//
// Created by nicol on 03/06/2024.
//

#include "Sphere.hpp"
#include "stb-master/stb_image.h"

Sphere::Sphere(float radius, GLuint slices, GLuint stacks, const std::string &texture_path) {
    _radius = radius;
    _slices = slices;
    _stacks = stacks;
    _texture_path = texture_path;
    initShader();
    setup();
}

void Sphere::setup() {
    _n_vertices = (_slices + 1) * (_stacks + 1);
    _n_elements = (_slices * 2 * (_stacks - 1)) * 3;

    // Verts
    auto vertices = new float[3 * _n_vertices];
    // Normals
    auto normals = new float[3 * _n_vertices];
    // Tex coords
    auto tex_coords = new float[2 * _n_vertices];

    // Index
    auto elements = new unsigned int[_n_elements];

    // Generate the vertex data
    generateVertices(vertices, normals, tex_coords, elements);

    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);

    glGenBuffers(1, &_vbo_position);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_position);
    glBufferData(GL_ARRAY_BUFFER, 3 * _n_vertices * sizeof(float), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &_vbo_normal);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_normal);
    glBufferData(GL_ARRAY_BUFFER, 3 * _n_vertices * sizeof(float), normals, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(1);

    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &_texture);
    glBindTexture(GL_TEXTURE_2D, _texture);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT); //GL_CLAMP
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT); //GL_CLAMP
    int width, height, n_channels;
    unsigned char *data = stbi_load(_texture_path.c_str(), &width, &height, &n_channels, 0);
    if (data == nullptr) {
        std::cerr << "Error loading texture " << _texture_path << std::endl;
        exit(1);
    }
    auto format = n_channels == 3 ? GL_RGB : GL_RGBA;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0,GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);


    glGenBuffers(1, &_vbo_tex_coord);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo_tex_coord);
    glBufferData(GL_ARRAY_BUFFER, 2 * _n_vertices * sizeof(float), tex_coords, GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(2);

    glGenBuffers(1, &_ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _n_elements * sizeof(unsigned int), elements, GL_STATIC_DRAW);


    glBindVertexArray(0);

    delete[] vertices;
    delete[] normals;
    delete[] tex_coords;
    delete[] elements;
}

void Sphere::initShader() {
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

void Sphere::generateVertices(float *vertices, float *normals, float *tex_coords, GLuint *elements) const {
    // Generate positions and normals
    GLfloat theta, phi;
    GLfloat theta_fac = glm::two_pi<float>() / _slices;
    GLfloat phi_fac = glm::pi<float>() / _stacks;
    GLfloat nx, ny, nz, s, t;
    GLuint idx = 0, tIdx = 0;
    for (GLuint i = 0; i <= _slices; i++) {
        theta = i * theta_fac;
        s = (GLfloat)i / _slices;
        for (GLuint j = 0; j <= _stacks; j++) {
            phi = j * phi_fac;
            t = (GLfloat)j / _stacks;
            nx = sinf(phi) * cosf(theta);
            ny = sinf(phi) * sinf(theta);
            nz = cosf(phi);
            vertices[idx] = _radius * nx; vertices[idx + 1] = _radius * ny; vertices[idx + 2] = _radius * nz;
            normals[idx] = nx; normals[idx + 1] = ny; normals[idx + 2] = nz;
            idx += 3;

            tex_coords[tIdx] = s;
            tex_coords[tIdx + 1] = t;
            tIdx += 2;
        }
    }

    // Generate the element list
    idx = 0;
    for (GLuint i = 0; i < _slices; i++) {
        GLuint stack_start = i * (_stacks + 1);
        GLuint next_stack_start = (i + 1) * (_stacks + 1);
        for (GLuint j = 0; j < _stacks; j++) {
            if (j == 0) {
                elements[idx] = stack_start;
                elements[idx + 1] = stack_start + 1;
                elements[idx + 2] = next_stack_start + 1;
                idx += 3;
            }
            else if (j == _stacks - 1) {
                elements[idx] = stack_start + j;
                elements[idx + 1] = stack_start + j + 1;
                elements[idx + 2] = next_stack_start + j;
                idx += 3;
            }
            else {
                elements[idx] = stack_start + j;
                elements[idx + 1] = stack_start + j + 1;
                elements[idx + 2] = next_stack_start + j + 1;
                elements[idx + 3] = next_stack_start + j;
                elements[idx + 4] = stack_start + j;
                elements[idx + 5] = next_stack_start + j + 1;
                idx += 6;
            }
        }
    }
}

void Sphere::draw(glm::mat4 view, glm::mat4 projection, Light light) const
{
    _shader->use();
    glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(_position.x, _position.y, _position.z));
    glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), glm::radians(static_cast<float>(_rotation.x)), glm::vec3(1.0f, 0.0f, 0.0f));
    rotate *= glm::rotate(glm::mat4(1.0f), glm::radians(static_cast<float>(_rotation.y)), glm::vec3(0.0f, 1.0f, 0.0f));
    rotate *= glm::rotate(glm::mat4(1.0f), glm::radians(static_cast<float>(_rotation.z)), glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(_scale, _scale, _scale));
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

void Sphere::setRadius(float radius) {
    _radius = radius;
    setup();
}
