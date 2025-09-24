#version 450

in vec3 ReflectDir;

uniform samplerCube CubeMapTex;

out vec4 FragColor;

void main() {
    // Access the cube map texture
    vec4 cubeMapColor = texture(CubeMapTex, ReflectDir);
    FragColor = cubeMapColor;
}
