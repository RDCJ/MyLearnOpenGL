#version 460 core
layout (location = 0) in vec3 aPos;

// 使用的是一个std140布局的Uniform块
layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};


uniform mat4 model;

void main()
{
    gl_Position = projection * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);
}