#version 460

out vec4 FragColor;

float near = 0.1;
float far = 100;

float LinerizeDepth(float depth){
    float z = depth * 2.0 -1.0;
    return (2.0 * near * far) / (far + near - z * (far - near));
}

void main()
{
    // gl_FragCoord的x和y分量代表了片段的屏幕空间坐标（其中(0, 0)位于左下角）。
    // gl_FragCoord中也包含了一个z分量，它包含了片段真正的深度值
    float depth = LinerizeDepth(gl_FragCoord.z) / far;
    FragColor = vec4(vec3(depth), 1.0);
}