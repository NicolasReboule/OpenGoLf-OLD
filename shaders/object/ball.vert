#version 450

layout(location=0) in vec3 VertexPosition;
layout(location=1) in vec3 VertexNormal;
layout(location=2) in vec2 VertexTexCoord;
out vec3 Position;
out vec3 Normal;
uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 MVP;
out vec2 TexCoord;

void main()
{
    TexCoord = VertexTexCoord;
    //Position in camera coordinates
    Position = vec3(ModelViewMatrix * vec4(VertexPosition, 1.0));
    //Transform the normal to camera coordinates
    Normal = NormalMatrix * VertexNormal;
    gl_Position = MVP * vec4(VertexPosition, 1.0);
}
