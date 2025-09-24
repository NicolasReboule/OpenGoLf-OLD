//
// Created by nicol on 17/05/2024.
//

#ifndef OPENGOLF_SKYBOX_HPP
#define OPENGOLF_SKYBOX_HPP

#include <vector>
#include <string>
#include <iostream>
#include <memory>
#include <GL/gl3w.h>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Loader.h"

class SkyBox {
public:
    SkyBox();

    void draw(glm::mat3 view, glm::mat4 projection) const;

private:
    void initShader();

    void loadCubeMap();
private:
    GLuint _vao = 0;

    GLuint _tex = 0;

    std::unique_ptr<ShaderProgram> _shader;
};


#endif //OPENGOLF_SKYBOX_HPP
