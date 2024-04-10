#version 460 core

in vec3 TexCoord;

out vec4 FragColor;

uniform samplerCube cube_map;

void main(){
    // 使用一个vec3的方向向量对samplerCube进行采样
    FragColor = texture(cube_map, TexCoord);
}