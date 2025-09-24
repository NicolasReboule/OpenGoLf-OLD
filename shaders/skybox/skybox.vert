#version 450

layout(location=0) in vec3 VertexPosition;
layout(location=1) in vec3 VertexNormal;

out vec3 ReflectDir;

uniform mat4 MVP;

void main()
{
    ReflectDir = VertexPosition;
    gl_Position = MVP * vec4(VertexPosition, 1.0);
}
