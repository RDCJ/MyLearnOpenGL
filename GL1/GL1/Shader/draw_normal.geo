#version 460 core

layout (triangles) in;
layout (line_strip, max_vertices=6) out;

// 使用的是一个std140布局的Uniform块
layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

in vec3 Normal[];

uniform float normal_length;

void main()
{
    for (int i=0; i<3; i++)
    {
        gl_Position = projection * gl_in[i].gl_Position;
        EmitVertex();
        gl_Position = projection * (gl_in[i].gl_Position + normal_length * vec4(Normal[i], 0.0));
        EmitVertex();

        EndPrimitive();
    }
}
