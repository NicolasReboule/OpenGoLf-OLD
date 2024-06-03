//
// Created by nicol on 17/05/2024.
//

#ifndef OPENGOLF_SKYBOX_HPP
#define OPENGOLF_SKYBOX_HPP

#include <vector>
#include <string>
#include <iostream>
#include <GL/gl3w.h>

class SkyBox {
public:
    SkyBox();

    void draw() const;

    void loadCubeMap();

    [[nodiscard]] inline GLuint getTexID() const { return texID; }
private:
    unsigned int vaoHandle = 0;

    GLuint texID = 0;
};


#endif //OPENGOLF_SKYBOX_HPP
