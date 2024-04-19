#version 460 core

in vec3 TexCoord;

out vec4 FragColor;

uniform samplerCube cube_map;
uniform float z_near;
uniform float z_far;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * z_near * z_far) / (z_far + z_near - z * (z_far - z_near));
}

void main(){
    // 使用一个vec3的方向向量对samplerCube进行采样
    float depth = texture(cube_map, TexCoord).r;
    depth = LinearizeDepth(depth);
    FragColor = vec4(vec3(depth), 1.0);
}