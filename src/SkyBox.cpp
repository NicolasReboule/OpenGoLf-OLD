//
// Created by nicol on 17/05/2024.
//

#include "SkyBox.hpp"
#include "stb-master/stb_image.h"

SkyBox::SkyBox() {
    initShader();
    loadCubeMap();
}

void SkyBox::initShader() {
    _shader = std::make_unique<ShaderProgram>();
    _shader->initFromFiles("shaders/skybox/skybox.vert", "shaders/skybox/skybox.frag");

    _shader->addUniform("MVP");

    _shader->addUniform("CubeMapTex"); //cubemap texture
}

void SkyBox::draw(glm::mat3 view, glm::mat4 projection) const {
    glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), glm::radians(static_cast<float>(0.0f)), glm::vec3(1.0f, 0.0f, 0.0f));
    rotate *= glm::rotate(glm::mat4(1.0f), glm::radians(static_cast<float>(0.0f)), glm::vec3(0.0f, 1.0f, 0.0f));
    rotate *= glm::rotate(glm::mat4(1.0f), glm::radians(static_cast<float>(0.0f)), glm::vec3(0.0f, 0.0f, 1.0f));    glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.5, 0.5, 0.5));
    glm::mat4 model = translate * rotate * scale; // Combination of transformation matrix

    glm::mat4 skybox_view = glm::mat4(glm::mat3(view));
    glm::mat4 mvp = projection * skybox_view * model;

    _shader->use();

    glUniformMatrix4fv(_shader->uniform("MVP"), 1, GL_FALSE, glm::value_ptr(mvp));

    glUniform1i(_shader->uniform("CubeMapTex"), 0);

    glDepthMask(GL_FALSE);
    if (_vao) {
        glBindVertexArray(_vao);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _tex);
        int size;
        glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
        glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_INT, nullptr);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
    }
    glDepthMask(GL_TRUE);
    _shader->disable();
}

void SkyBox::loadCubeMap() {
    float size = 500.0f; //size of cube
    float half_size = size / 2.0f;
    float vertices[24 * 3] = {
// Front
            -half_size, -half_size, half_size,
            half_size, -half_size, half_size,
            half_size, half_size, half_size,
            -half_size, half_size, half_size,
// Right
            half_size, -half_size, half_size,
            half_size, -half_size, -half_size,
            half_size, half_size, -half_size,
            half_size, half_size, half_size,
// Back
            -half_size, -half_size, -half_size,
            -half_size, half_size, -half_size,
            half_size, half_size, -half_size,
            half_size, -half_size, -half_size,
// Left
            -half_size, -half_size, half_size,
            -half_size, half_size, half_size,
            -half_size, half_size, -half_size,
            -half_size, -half_size, -half_size,
// Bottom
            -half_size, -half_size, half_size,
            -half_size, -half_size, -half_size,
            half_size, -half_size, -half_size,
            half_size, -half_size, half_size,
// Top
            -half_size, half_size, half_size,
            half_size, half_size, half_size,
            half_size, half_size, -half_size,
            -half_size, half_size, -half_size
    };
    GLuint elements[] = {
            0,2,1, 0,3,2, 4,6,5, 4,7,6,
            8,10,9, 8,11,10, 12,14,13, 12,15,14,
            16,18,17, 16,19,18, 20,22,21, 20,23,22
    };

    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);

    GLuint vbo_vertices_handle;
    glGenBuffers(1, &vbo_vertices_handle);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices_handle);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    GLuint vbo_elements_handle;
    glGenBuffers(1, &vbo_elements_handle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_elements_handle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), &elements, GL_STATIC_DRAW);

    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &_tex); //set the texID as a member variable
    glBindTexture(GL_TEXTURE_CUBE_MAP, _tex);

    const char *name[] = { "skybox_left", "skybox_right", "skybox_top", "skybox_bottom", "skybox_back", "skybox_front" };
    GLuint targets[] = {
            GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
            GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
            GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
    };
    for (int i = 0; i < 6; i++) {
        int channel;
        int width, height;
        std::string filename;
        filename = std::string("assets/skybox/") + std::string(name[i]) + std::string(".png"); //or PNG
        unsigned char *image = stbi_load(filename.c_str(), &width, &height, &channel, 0);
        if (image == nullptr) {
            std::cerr << "Error loading texture " << filename << std::endl;
            exit(1);
        }
        auto format = channel == 3 ? GL_RGB : GL_RGBA;
        glTexImage2D(targets[i], 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(image);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindVertexArray(0);
}
