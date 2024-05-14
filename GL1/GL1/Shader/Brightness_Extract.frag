#version 460 core

in vec2 TexCoord;
out vec4 FragColor;
uniform sampler2D tex;

void main()
{
    const vec3 weight = vec3(0.2126, 0.7152, 0.0722);
    vec3 color = texture(tex, TexCoord).rgb;
    float brightness = dot(color, weight);
    if (brightness > 1.0)
    {
        FragColor = vec4(color, 1.0);
    }
    else
    {
        FragColor = vec4(0, 0, 0, 1);
    }
}