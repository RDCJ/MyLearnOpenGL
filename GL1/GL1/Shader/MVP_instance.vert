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

uniform mat4 model[100];
uniform mat3 NormalMatrix[100];

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoord;

void main()
{
    // gl_InstanceID是顶点着色器中的一个内建变量，
    // 在使用实例化渲染调用时，gl_InstanceID会从0开始，在每个实例被渲染时递增1
    gl_Position = projection * view * model[gl_InstanceID] * vec4(aPos.x, aPos.y, aPos.z, 1.0);

    // 需要世界空间中进行所有的光照计算，因此我们需要一个在世界空间中的顶点位置。
    // 可以通过把顶点位置属性乘以模型矩阵来把它变换到世界空间坐标
    FragPos = vec3(model[gl_InstanceID] * vec4(aPos, 1.0));

    // 法线矩阵
    Normal = NormalMatrix[gl_InstanceID] * aNormal;

    TexCoord = aTexCoord;
}