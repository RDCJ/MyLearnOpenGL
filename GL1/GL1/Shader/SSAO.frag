#version 460 core

in vec2 TexCoord;
out float FragColor;

struct GBuffer
{
    sampler2D gViewPosition;
    sampler2D gViewNormal;
};

uniform GBuffer g_buffer;
uniform sampler2D SSAONoise;
uniform vec3 SSAOKernel[64];

// kernelSize和radius用来调整SSAO的效果
uniform int kernel_size;
uniform float radius;

// 使用的是一个std140布局的Uniform块
layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

uniform vec2 screen_size;

void main()
{
    const vec2 noiseScale = screen_size * 0.25;
    vec3 FragPos = texture(g_buffer.gViewPosition, TexCoord).xyz;
    vec3 normal = texture(g_buffer.gViewNormal, TexCoord).rgb;
    vec3 randomVec = texture(SSAONoise, noiseScale * TexCoord).xyz;
    
    // 创建一个TBN矩阵，将向量从切线空间变换到观察空间
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    float occlusion = 0;
    for (int i=0; i<kernel_size; i++)
    {
        vec3 sample_v3 = FragPos + radius * TBN * SSAOKernel[i]; // 切线空间->观察空间
        vec4 sample_v4 = vec4(sample_v3, 1.0);
        // 观察空间 -> 裁剪空间
        sample_v4 = projection * sample_v4;
        // 透视划分，裁剪空间 -> 屏幕空间
        sample_v4.xyz /= sample_v4.w;
        // 变换到0.0 - 1.0的值域
        sample_v4.xyz = sample_v4.xyz * 0.5 + 0.5;
        // 对深度采样
        float sampleDepth = -texture(g_buffer.gViewPosition, sample_v4.xy).w;

        float range_check = smoothstep(0, 1, radius / abs(FragPos.z - sampleDepth));
        occlusion += (sampleDepth >= sample_v3.z ? 1.0 : 0.0) * range_check;
    }
    FragColor = 1.0 - (occlusion / kernel_size);
}