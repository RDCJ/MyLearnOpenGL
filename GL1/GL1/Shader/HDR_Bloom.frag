#version 460 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D tex;
uniform sampler2D bloom_tex;

uniform float exposure;

void main()
{
    const float gamma = 2.2;
    vec3 hdr_color = texture(tex, TexCoord).rgb + 0.5 * texture(bloom_tex, TexCoord).rgb;
    vec3 mapped = vec3(1) - exp(-hdr_color * exposure);
    //mapped = pow(mapped, vec3(1.0 / gamma));
    FragColor = vec4(mapped, 1.0);
}