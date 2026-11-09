#version 330 core

out vec4 FragColor;

in vec2 vTexCoord;

uniform sampler2D uTexture;
uniform vec3 uColor;
uniform int uHasColor;
uniform int uMix;

void main()
{
    vec4 tex = texture(uTexture, vTexCoord);
    vec4 color = vec4(uColor, 1.0);

    if (uHasColor == 0)
    FragColor = tex;
    else if (uMix == 1)
    FragColor = mix(tex, color, 0.5);
    else
    FragColor = tex * color;
}