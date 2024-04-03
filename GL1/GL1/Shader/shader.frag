#version 460 core

struct Material
{
    // 漫反射贴图
    sampler2D diffuse;
    // 镜面反射贴图
    sampler2D specular;
    // 放射光贴图
    sampler2D emission;
    // 高光的反光度(Shininess)。一个物体的反光度越高，反射光的能力越强，散射得越少，高光点就会越小
    float shininess;
};

struct Light
{
    int type;
    vec3 position;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main()
{   
    // 标准化
    vec3 norm = normalize(Normal);
    // 计算光照方向: 计算需求一个从片段至光源的光线方向
    vec3 lightDir = vec3(0);
    if (light.type == 0)
    {
        lightDir = -light.direction;
    }
    else if (light.type == 1)
    {
        lightDir = light.position - FragPos;
    }
    lightDir = normalize(lightDir);

    // 环境光照
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));

    // 漫反射光照：使物体上法线方向与光线方向越接近的片段能从光源处获得更多的亮度
    // 用点乘表示法线方向与光线方向的接近程度：两个单位向量的夹角越小，它们点乘的结果越倾向于1
    // 如果两个向量之间的角度大于90度，点乘的结果就会变成负数，这样会导致漫反射分量变为负数
    // 一般来说，物体表面只反射照在正面的光线，因此排除负数的情况
    float diffuse_param = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diffuse_param * light.diffuse * vec3(texture(material.diffuse, TexCoord));

    // 镜面光照：当观察方向与反射方向接近时，可以看到高光
    vec3 viewDir = normalize(viewPos - FragPos);
    // 先用法向量翻折入射光的方向来计算反射向量。
    // reflect函数要求第一个向量是从光源指向片段位置的向量
    vec3 reflectDir = reflect(-lightDir, norm);
    // 然后计算反射向量与观察方向的角度差，它们之间夹角越小，镜面光的作用就越大, 用点乘表示角度差
    float specular_param = pow(max(dot(reflectDir, viewDir), 0.0), material.shininess);
    vec3 specular = specular_param * light.specular * vec3(texture(material.specular, TexCoord));

    //vec3 emission = vec3(texture(material.emission, TexCoord));
    
    vec3 result = ambient + diffuse + specular;// + emission;
    FragColor = vec4(result, 1.0f);
} 