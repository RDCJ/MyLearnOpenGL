#version 460 core

in vec2 TexCoord;
out float FragColor;

uniform sampler2D tex;

void main()
{
    vec2 texSize = 1.0 / vec2(textureSize(tex, 0));
    float result = 0;
    for (int x=-2; x<2; x++)
    {
        for (int y=-2; y<2; y++)
        {
            vec2 offset = vec2(float(x), float(y)) * texSize;
            result += texture(tex, TexCoord + offset).r;
        }
    }
    FragColor = result / 16.0;
}