#version 460 core

in vec2 TexCoord;
out vec4 FragColor;

struct GBuffer
{
    sampler2D gPosition;
    sampler2D gNormal;
    sampler2D gDiffuse;
    sampler2D gSpecular;
};

uniform GBuffer g_buffer;

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

#define LIGHT_MAX_NUM 6
uniform Light lights[LIGHT_MAX_NUM];
uniform int light_num;
uniform vec3 viewPos;

vec3 FragPos;
vec3 Normal;
vec3 diffuse_v;
vec3 specular_v;
float shininess;

vec3 CalcLight(int light_idx, vec3 normal, vec3 viewDir)
{
    Light light = lights[light_idx];
    // 计算从片段至光源的光线方向
    vec3 lightDir = float(light.type == 0) * (-light.direction) + 
                    float(light.type != 0) * (light.position - FragPos);
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
    vec3 ambient = light.ambient * diffuse_v;

    // 漫反射光照：使物体上法线方向与光线方向越接近的片段能从光源处获得更多的亮度
    // 用点乘表示法线方向与光线方向的接近程度：两个单位向量的夹角越小，它们点乘的结果越倾向于1
    // 如果两个向量之间的角度大于90度，点乘的结果就会变成负数，这样会导致漫反射分量变为负数
    // 一般来说，物体表面只反射照在正面的光线，因此排除负数的情况
    float diffuse_param = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diffuse_param * light.diffuse * diffuse_v;

    // 镜面光照：当观察方向与反射方向接近时，可以看到高光
    // 计算半程向量
    vec3 halfwayDir = normalize(lightDir + viewDir);
    // 然后计算半程向量与法向量的角度差，它们之间夹角越小，镜面光的作用就越大, 用点乘表示角度差
    float specular_param = pow(max(dot(halfwayDir, normal), 0.0), shininess);

    vec3 specular = specular_param * light.specular * specular_v;

    //vec3 emission = vec3(texture(material.emission, TexCoord));
    
    return attenuation * (ambient + spot_intensity * (diffuse + specular));// + emission;
}

void main()
{
    FragPos = texture(g_buffer.gPosition, TexCoord).rgb;
    Normal = texture(g_buffer.gNormal, TexCoord).rgb;
    diffuse_v = texture(g_buffer.gDiffuse, TexCoord).rgb;
    specular_v = texture(g_buffer.gSpecular, TexCoord).rgb;
    shininess = texture(g_buffer.gSpecular, TexCoord).a;

    // 观察方向(片段->摄像机)
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = vec3(0);
    for (int i=0; i<light_num; i++)
        result += CalcLight(i, Normal, viewDir);
    FragColor = vec4(result, 1.0f);
}