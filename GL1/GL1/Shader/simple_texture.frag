#version 460 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D tex;

void main(){
    FragColor = vec4(1 - vec3(texture(tex, TexCoord)), 1.0);
}