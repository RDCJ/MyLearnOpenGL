#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 Tangent;
layout (location = 4) in vec3 Bitangent;

// 使用的是一个std140布局的Uniform块
layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

uniform mat4 model;
//uniform mat4 view;
//uniform mat4 projection;
uniform mat3 NormalMatrix;
uniform int use_normal_map;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoord;
out mat3 TBN;

void main()
{
    gl_Position = projection * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);

    // 需要世界空间中进行所有的光照计算，因此我们需要一个在世界空间中的顶点位置。
    // 可以通过把顶点位置属性乘以模型矩阵来把它变换到世界空间坐标
    FragPos = vec3(model * vec4(aPos, 1.0));

    // 法线矩阵
    Normal = NormalMatrix * aNormal;

    TexCoord = aTexCoord;

    TBN = mat3(1);
    if (use_normal_map == 1)
    {
        // TBN
        vec3 T = normalize(vec3(model * vec4(Tangent, 0)));
        vec3 B = normalize(vec3(model * vec4(Bitangent, 0)));
        vec3 N = normalize(Normal);
        TBN = mat3(T, B, N);
    }
}