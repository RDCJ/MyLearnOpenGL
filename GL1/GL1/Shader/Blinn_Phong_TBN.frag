#version 460 core

struct Material
{
    // 漫反射贴图
    sampler2D texture_diffuse0;
    sampler2D texture_diffuse1;
    sampler2D texture_diffuse2;
    // 镜面反射贴图
    sampler2D texture_specular0;
    sampler2D texture_specular1;
    sampler2D texture_specular2;
    // 放射光贴图
    sampler2D texture_emission0;
    sampler2D texture_emission1;
    sampler2D texture_emission2;

    sampler2D texture_normal0;

    sampler2D texture_ambient0;
    // 高光的反光度(Shininess)。一个物体的反光度越高，反射光的能力越强，散射得越少，高光点就会越小
    float shininess;

    float refract_ratio;
};

struct Light
{
    int type;
    vec3 position;
    vec3 direction;

    float innerCutOff;
    float outerCutOff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
in mat3 TBN;

out vec4 FragColor;

uniform int use_blinn;
uniform int light_num;
uniform vec3 viewPos;
uniform Material material;
uniform int use_normal_map;
uniform int use_cube_map;
uniform samplerCube cube_map;
uniform float refract_ratio;

#define LIGHT_MAX_NUM 6
uniform Light lights[LIGHT_MAX_NUM];


vec3 CalcLight(Light light, vec3 normal, vec3 viewDir)
{
    // 计算从片段至光源的光线方向
    vec3 lightDir = vec3(0);
    if (light.type == 0)
    {
        lightDir = -light.direction;
    }
    else
    {
        lightDir = light.position - FragPos;
    }
    lightDir = normalize(lightDir);

    // 聚光光源计算强度
    // 内圆锥之内强度为1，内圆锥到外圆锥之间强度由1减小到0
    float spot_intensity = 1;
    if (light.type == 2)
    {
        // theta = cos(光照方向, 光源到片段之间的方向)
        float theta = dot(-lightDir, normalize(light.direction));
        float epsilon = light.innerCutOff - light.outerCutOff;
        spot_intensity = clamp((theta - light.outerCutOff) / epsilon, 0, 1);
    }

    // 计算光照衰减
    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);

    // 环境光照
    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse0, TexCoord));

    // 漫反射光照：使物体上法线方向与光线方向越接近的片段能从光源处获得更多的亮度
    // 用点乘表示法线方向与光线方向的接近程度：两个单位向量的夹角越小，它们点乘的结果越倾向于1
    // 如果两个向量之间的角度大于90度，点乘的结果就会变成负数，这样会导致漫反射分量变为负数
    // 一般来说，物体表面只反射照在正面的光线，因此排除负数的情况
    float diffuse_param = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diffuse_param * light.diffuse * vec3(texture(material.texture_diffuse0, TexCoord));

    // 镜面光照：当观察方向与反射方向接近时，可以看到高光
    float specular_param = 0;
    if (use_blinn > 0)
    {
        // 计算半程向量
        vec3 halfwayDir = normalize(lightDir + viewDir);
        // 然后计算半程向量与法向量的角度差，它们之间夹角越小，镜面光的作用就越大, 用点乘表示角度差
        specular_param = pow(max(dot(halfwayDir, normal), 0.0), material.shininess);
    }
    else
    {
        // 先用法向量翻折入射光的方向来计算反射向量。
        // reflect函数要求第一个向量是从光源指向片段位置的向量
        vec3 reflectDir = reflect(-lightDir, normal);
        // 然后计算反射向量与观察方向的角度差，它们之间夹角越小，镜面光的作用就越大, 用点乘表示角度差
        specular_param = pow(max(dot(reflectDir, viewDir), 0.0), material.shininess);
    }
    vec3 specular = specular_param * light.specular * vec3(texture(material.texture_specular0, TexCoord));

    //vec3 emission = vec3(texture(material.emission, TexCoord));
    
    return attenuation * (ambient + spot_intensity * (diffuse + specular));// + emission;
}

vec4 CalcEnvironmentReflection(vec3 normal, vec3 viewDir)
{
    // 计算由观察方向(摄像机->片段)关于法线的反射方向
    vec3 reflectDir = reflect(viewDir, normal);
    // 用反射方向在立方体贴图上采样
    return texture(material.texture_ambient0, TexCoord) * texture(cube_map, reflectDir);
    //return texture(cube_map, reflectDir);
}

vec4 CalcEnvironmentRefraction(vec3 normal, vec3 viewDir)
{
    vec3 refractDir = refract(viewDir, normal, material.refract_ratio);
    return texture(cube_map, refractDir);
}

void main()
{   
    // 法线标准化
    vec3 norm = normalize(Normal);
    // 如果材质中有法线贴图
    if (use_normal_map == 1)
    {
        // 从法线贴图采样获取法线
        norm = texture(material.texture_normal0, TexCoord).rgb;
        // 将法线向量转换为范围[-1,1]
        norm = normalize(norm * 2.0 - 1.0);
        norm = normalize(TBN * norm);
    }
    // 观察方向(片段->摄像机)
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = vec3(0);

    for (int i=0; i<light_num; i++)
        result += CalcLight(lights[i], norm, viewDir);
    
    if (use_cube_map == 1)
    {
        result += vec3(CalcEnvironmentReflection(norm, -viewDir));
        //result += vec3(CalcEnvironmentRefraction(norm, -viewDir));
    }
    FragColor = vec4(result, 1.0f);
} 