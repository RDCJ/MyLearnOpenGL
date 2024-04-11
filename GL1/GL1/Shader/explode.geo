#version 460 core

// 几何着色器的输入是一个图元的所有顶点
// 在in关键字前声明一个布局修饰符, 指定从顶点着色器接收的图元类型
// 可选择的修饰符: points, lines, lines_adjacency, triangles, triangles_adjacency
layout (triangles) in;
// 在out关键字前面加一个布局修饰符，指定几何着色器输出的图元类型
// 可选择的修饰符：points, lines_strip, triangle_strip
// max_vertices设置最大能够输出的顶点数量（如果这个值，OpenGL将不会绘制多出的顶点)
layout (triangle_strip, max_vertices=3) out;

in vec2 TexCoord[];
in vec3 FragPos[];
in vec3 Normal[];

out vec2 TexCoord_;
out vec3 FragPos_;
out vec3 Normal_;

uniform float explode_length;

// 计算三角形的法向量
vec3 GetNormal()
{
   vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
   vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
   return normalize(cross(a, b));
}

// 让位置向量沿着法线向量进行位移
vec4 Explode(vec4 position, vec3 normal)
{
    return position + explode_length * vec4(normal, 0);
}

void main()
{
    vec3 normal = GetNormal();

    // 
    gl_Position = Explode(gl_in[0].gl_Position, normal);
    TexCoord_ = TexCoord[0];
    FragPos_ = FragPos[0];
    Normal_ = Normal[0];
    EmitVertex();
    // 每次我们调用EmitVertex时，gl_Position中的向量会被添加到图元中来

    gl_Position = Explode(gl_in[1].gl_Position, normal);
    TexCoord_ = TexCoord[1];
    FragPos_ = FragPos[1];
    Normal_ = Normal[1];
    EmitVertex();

    gl_Position = Explode(gl_in[2].gl_Position, normal);
    TexCoord_ = TexCoord[2];
    FragPos_ = FragPos[2];
    Normal_ = Normal[2];
    EmitVertex();

    // 当EndPrimitive被调用时，所有发射出的(Emitted)顶点都会合成为指定的输出渲染图元
    // 在一个或多个EmitVertex调用之后重复调用EndPrimitive能够生成多个图元
    EndPrimitive();
}