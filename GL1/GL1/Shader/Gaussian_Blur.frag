#version 460 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D tex;
uniform int horizontal;
float weight[5] = float[]
(
    0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216
);

void main()
{
    vec2 tex_delta = 1.0 / textureSize(tex, 0);
    vec2 deltaX = vec2(tex_delta.x, 0);
    vec2 deltaY = vec2(0, tex_delta.y);
    vec3 color = texture(tex, TexCoord).rgb * weight[0];
    if (horizontal == 0)
    {
        for (int i=1; i<5; i++)
        {
            color += texture(tex, TexCoord + i * deltaX).rgb * weight[i];
            color += texture(tex, TexCoord - i * deltaX).rgb * weight[i];
        }
    }
    else
    {
        for (int i=1; i<5; i++)
        {
            color += texture(tex, TexCoord + i * deltaY).rgb * weight[i];
            color += texture(tex, TexCoord - i * deltaY).rgb * weight[i];
        }
    }
    
    FragColor = vec4(color, 1.0);
}