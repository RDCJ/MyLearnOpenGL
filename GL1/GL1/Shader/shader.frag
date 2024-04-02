#version 460 core

in vec3 FragPos;
in vec3 Normal;
out vec4 FragColor;

uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform vec3 ourColor;

void main()
{
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
    

    // 漫反射光照使物体上法线方向与光线方向越接近的片段能从光源处获得更多的亮度
    // 标准化
    vec3 norm = normalize(Normal);
    // 计算光照方向
    vec3 lightDir = normalize(lightPosition - FragPos);
    // 用点乘表示法线方向与光线方向的接近程度：两个单位向量的夹角越小，它们点乘的结果越倾向于1
    // 如果两个向量之间的角度大于90度，点乘的结果就会变成负数，这样会导致漫反射分量变为负数
    // 一般来说，物体表面只反射照在正面的光线，因此排除负数的情况
    float diffuse_param = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diffuse_param * lightColor;

    vec3 result = (ambient + diffuse) * ourColor;
    FragColor = vec4(result, 1.0f);
} 