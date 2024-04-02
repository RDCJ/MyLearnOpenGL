#version 460 core

out vec4 FragColor;

uniform vec3 lightColor;
uniform vec3 ourColor;

void main()
{
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
    FragColor = vec4(ourColor * ambient, 1.0f);
} 