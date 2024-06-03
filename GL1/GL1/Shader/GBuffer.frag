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
uniform float z_far;
uniform float z_near;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // 回到NDC
    return (2.0 * z_near * z_far) / (z_far + z_near - z * (z_far - z_near));    
}

void main()
{
    gPosition = vec4(FragPos, LinearizeDepth(gl_FragCoord.z));
    gNormal = vec4(normalize(Normal), 1.0);
    gDiffuse = vec4(texture(material.texture_diffuse0, TexCoord).rgb, 1.0);
    gSpecular = vec4(texture(material.texture_specular0, TexCoord).rgb, material.shininess);
}