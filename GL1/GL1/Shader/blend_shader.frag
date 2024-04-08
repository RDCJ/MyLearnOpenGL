#version 460

struct Material
{
    // 漫反射贴图
    sampler2D texture_diffuse0;
};

uniform Material material;

in vec2 TexCoord;
out vec4 FragColor;

void main(){
    vec4 tex_color = texture(material.texture_diffuse0, TexCoord);
    if (tex_color.a < 0.1) discard;
    FragColor = tex_color;
}

