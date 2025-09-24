#version 450

layout(location = 0) in vec4 vertexPosition; //attribute number 0
layout(location = 1) in vec3 vertexColor; //attribute number 1

//uniform mat4 model;
//uniform mat4 view;
//uniform mat4 projection;
uniform mat4 MVP;

out vec3 fcolor;

void main(void)
{
    fcolor = vertexColor;
    //clip coordinates
    gl_Position = MVP * vertexPosition;
}