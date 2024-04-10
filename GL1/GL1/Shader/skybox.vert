#version 460 core

layout (location = 0) in vec3 aPos;

out vec3 TexCoord;

uniform mat4 projection;
uniform mat4 view;

void main(){
    // 使用立方体的位置作为纹理坐标来采样。
    // 当立方体处于原点(0, 0, 0)时，它的每一个位置向量都是从原点出发的方向向量。这个方向向量正是获取立方体上特定位置的纹理值所需要的
    TexCoord = aPos;
    vec4 pos = projection * view * vec4(aPos, 1.0);

    // 强制让天空盒有着最大的深度值1.0，只要它前面有一个物体，深度测试就会失败
    // 透视除法是在顶点着色器运行之后执行的，将gl_Position的xyz坐标除以w分量，相除结果的z分量等于顶点的深度值。
    // 将输出位置的z分量等于它的w分量，让z分量永远等于1.0，
    // 这样子的话，当透视除法执行之后，z分量会变为w / w = 1.0
    gl_Position = pos.xyww;

}