#version 460 core

out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D tex;
uniform float z_near;
uniform float z_far;


float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * z_near * z_far) / (z_far + z_near - z * (z_far - z_near));
}

void main()
{
    float depth = texture(tex, TexCoord).r;
    depth = LinearizeDepth(depth);
    FragColor = vec4(vec3(depth), 1.0);
}