#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

// 使用的是一个std140布局的Uniform块
layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

uniform mat4 model;
uniform mat3 NormalMatrix;

out vec3 Normal;

void main()
{
    gl_Position = view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);

    // 法线矩阵
    Normal = normalize(NormalMatrix * aNormal);
}