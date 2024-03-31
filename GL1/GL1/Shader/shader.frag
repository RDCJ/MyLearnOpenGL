#version 460 core

out vec4 FragColor;
in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;

uniform float mix_param;

void main()
{
    vec2 texcoord2 = vec2(1-TexCoord.x, TexCoord.y);
    //FragColor = texture(ourTexture, TexCoord);
    FragColor = mix(texture(texture1, TexCoord), texture(texture2, texcoord2), mix_param);
}