#version 330 core

in vec2 vTexCoord;
out vec4 FragColor;

uniform sampler2D uTexture;
uniform int uRed;

void main()
{
    if (uRed == 1)
    FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    else
    FragColor = texture(uTexture, vTexCoord);
}