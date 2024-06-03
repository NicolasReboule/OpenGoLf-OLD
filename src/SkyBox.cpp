//
// Created by nicol on 17/05/2024.
//

#include "SkyBox.hpp"
#include "stb-master/stb_image.h"

SkyBox::SkyBox() {
    loadCubeMap();
}

void SkyBox::draw() const {
    glBindVertexArray(vaoHandle);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texID);
    int size;
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_INT, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
}

void SkyBox::loadCubeMap() {
    float side = 500.0f; //size of cube
    float side2 = side / 2.0f;
    float v[24*3] = {
// Front
            -side2, -side2, side2,
            side2, -side2, side2,
            side2, side2, side2,
            -side2, side2, side2,
// Right
            side2, -side2, side2,
            side2, -side2, -side2,
            side2, side2, -side2,
            side2, side2, side2,
// Back
            -side2, -side2, -side2,
            -side2, side2, -side2,
            side2, side2, -side2,
            side2, -side2, -side2,
// Left
            -side2, -side2, side2,
            -side2, side2, side2,
            -side2, side2, -side2,
            -side2, -side2, -side2,
// Bottom
            -side2, -side2, side2,
            -side2, -side2, -side2,
            side2, -side2, -side2,
            side2, -side2, side2,
// Top
            -side2, side2, side2,
            side2, side2, side2,
            side2, side2, -side2,
            -side2, side2, -side2
    };
    GLuint el[] = {
            0,2,1, 0,3,2, 4,6,5, 4,7,6,
            8,10,9, 8,11,10, 12,14,13, 12,15,14,
            16,18,17, 16,19,18, 20,22,21, 20,23,22
    };

    glGenVertexArrays(1, &vaoHandle);
    glBindVertexArray(vaoHandle);

    GLuint vboVerticesHandle;
    glGenBuffers(1, &vboVerticesHandle);
    glBindBuffer(GL_ARRAY_BUFFER, vboVerticesHandle);
    glBufferData(GL_ARRAY_BUFFER, sizeof(v), &v, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    GLuint vboIndicesHandle;
    glGenBuffers(1, &vboIndicesHandle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndicesHandle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(el), &el, GL_STATIC_DRAW);

    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &texID); //set the texID as a member variable
    glBindTexture(GL_TEXTURE_CUBE_MAP, texID);

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