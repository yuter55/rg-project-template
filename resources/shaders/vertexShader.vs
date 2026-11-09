#version 330 core

layout (location = 0) in vec3 aPos;

uniform vec2 uOff;

void main()
{
    gl_Position = vec4(aPos.xy + uOff, aPos.z, 1.0);
}