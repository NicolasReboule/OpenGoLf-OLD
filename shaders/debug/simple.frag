#version 450

in vec3 fcolor; // must match the name in the vertex shader, only difference is in vs it's out
out vec4 fragColor; // must have at least one vec4 output

void main()
{
    fragColor = vec4(fcolor, 1.0f); // set the output to the color specified in the vertex shader
}