#version 460 core

in vec4 FragPos;

uniform vec3 light_pos;
uniform float z_far;

void main()
{
    float light_distance = length(FragPos.xyz - light_pos);
    gl_FragDepth = light_distance / z_far;
}