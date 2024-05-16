#version 460 core

layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gDiffuse;
layout (location = 3) out vec4 gSpecular;

struct Material
{
    // 漫反射贴图
    sampler2D texture_diffuse0;
    // 镜面反射贴图
    sampler2D texture_specular0;
    // 放射光贴图
    //sampler2D texture_emission0;

    //sampler2D texture_normal0;

    //sampler2D texture_ambient0;
    // 高光的反光度(Shininess)。一个物体的反光度越高，反射光的能力越强，散射得越少，高光点就会越小
    float shininess;

    float refract_ratio;
};

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

uniform Material material;

void main()
{
    gPosition = vec4(FragPos, 1.0);
    gNormal = vec4(normalize(Normal), 1.0);
    gDiffuse = texture(material.texture_diffuse0, TexCoord);
    gSpecular = texture(material.texture_specular0, TexCoord);
}