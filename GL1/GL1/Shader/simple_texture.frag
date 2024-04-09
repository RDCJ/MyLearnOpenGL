#version 460 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D tex;

void main(){
    // 移除场景中除了黑白灰以外所有的颜色，让整个图像灰度化
    FragColor = texture(tex, TexCoord);
    // 人眼会对绿色更加敏感一些，而对蓝色不那么敏感，所以为了获取物理上更精确的效果，我们需要使用加权的(Weighted)通道
    vec3 weight = vec3(0.2126, 0.7152, 0.0722);
    float average = dot(weight, vec3(FragColor));
    FragColor = vec4(average, average, average, 1.0);
}