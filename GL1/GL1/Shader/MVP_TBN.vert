#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 Tangent;

// 使用的是一个std140布局的Uniform块
layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
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

uniform mat4 model;
uniform mat3 NormalMatrix;
uniform int use_normal_map;
uniform vec3 viewPos;

uniform int light_num;
#define LIGHT_MAX_NUM 6
uniform Light lights[LIGHT_MAX_NUM];

out vec3 FragPos;
out vec2 TexCoord;
out vec3 Normal;

out vec3 TangentLightDir[LIGHT_MAX_NUM];
out vec3 TangentFragPos;
out vec3 TangentViewPos;

void main()
{
    gl_Position = projection * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);

    // 需要世界空间中进行所有的光照计算，因此我们需要一个在世界空间中的顶点位置。
    // 可以通过把顶点位置属性乘以模型矩阵来把它变换到世界空间坐标
    FragPos = vec3(model * vec4(aPos, 1.0));

    TexCoord = aTexCoord;

    Normal = NormalMatrix * aNormal;
/*
    // TBN
    vec3 T = normalize(NormalMatrix * Tangent);
    vec3 N = normalize(NormalMatrix * aNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    // 正交矩阵转置等于逆
    mat3 TBN_inverse = transpose(mat3(T, B, N));


    TangentFragPos = TBN_inverse * FragPos;
    TangentViewPos = TBN_inverse * viewPos;
    for (int i=0; i<light_num; i++)
    {
        vec3 lightDir = float(lights[i].type == 0) * (-lights[i].direction) + 
                        float(lights[i].type != 0) * (lights[i].position - FragPos);
        TangentLightDir[i] = normalize(TBN_inverse * lightDir);
    }
    */
    TangentFragPos = FragPos;
    TangentViewPos = viewPos;
    for (int i=0; i<light_num; i++)
    {
        vec3 lightDir = float(lights[i].type == 0) * (-lights[i].direction) + 
                        float(lights[i].type != 0) * (lights[i].position - FragPos);
        TangentLightDir[i] = normalize(lightDir);
    }
}