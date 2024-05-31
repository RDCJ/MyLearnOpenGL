#version 460 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D tex;

void main()
{
    float red = texture(tex, TexCoord).r;
    FragColor = vec4(red, red, red, 1.0);
}