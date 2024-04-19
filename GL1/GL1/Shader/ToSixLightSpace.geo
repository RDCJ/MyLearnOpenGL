#version 460 core

// 几何着色器负责将顶点变换到光空间
// 几何着色器以3个三角形的顶点作为输入, 输出总共6个三角形（6*3顶点，所以总共18个顶点）
layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

uniform mat4 lightSpaceMat[6];

out vec4 FragPos;

void main()
{
    // 将每个世界空间顶点变换6个光空间
    for (int face=0; face<6; face++)
    {
        // 几何着色器的内建变量gl_Layer，它指定发散出基本图形送到立方体贴图的哪个面
        gl_Layer = face;
        for (int i=0; i<3; i++)
        {
            FragPos = gl_in[i].gl_Position;
            gl_Position = lightSpaceMat[face] * FragPos;
            EmitVertex();
        }
        EndPrimitive();
    }
}