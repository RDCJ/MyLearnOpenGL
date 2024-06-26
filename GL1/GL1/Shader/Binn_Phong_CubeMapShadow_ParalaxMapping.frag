#version 460 core

struct Material
{
    // 漫反射贴图
    sampler2D texture_diffuse0;
    sampler2D texture_diffuse1;
    sampler2D texture_diffuse2;
    // 镜面反射贴图
    sampler2D texture_specular0;
    sampler2D texture_specular1;
    sampler2D texture_specular2;
    // 放射光贴图
    sampler2D texture_emission0;
    sampler2D texture_emission1;
    sampler2D texture_emission2;

    // 法线贴图
    sampler2D texture_normal0;
    // 视差贴图
    sampler2D texture_parallax0;

    sampler2D texture_ambient0;
    // 高光的反光度(Shininess)。一个物体的反光度越高，反射光的能力越强，散射得越少，高光点就会越小
    float shininess;

    float refract_ratio;
};

struct Light
{
    int type;
    vec3 position;
    vec3 direction;

    float innerCutOff;
    float outerCutOff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};
#define LIGHT_MAX_NUM 6

in vec3 FragPos;
in vec2 TexCoord;
in vec3 TangentLightDir[LIGHT_MAX_NUM];
in vec3 TangentFragPos;
in vec3 TangentViewPos;

uniform int use_blinn;
uniform int light_num;
uniform float z_far;
uniform Material material;
uniform int use_cube_map;
uniform samplerCube cube_map;
uniform float refract_ratio;
uniform float parallax_height_scale;

uniform Light lights[LIGHT_MAX_NUM];
uniform samplerCube cube_map_shadow[LIGHT_MAX_NUM];
out vec4 FragColor;

vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);

// 计算阴影的强度
// lightDir和normal要在同一坐标系下
// light_pos和FragPos要在世界坐标系下
float CalcShadow(int light_idx, vec3 lightDir, vec3 light_pos, vec3 normal)
{
    vec3 frag2light = FragPos - light_pos;
    // 获取当前fragment和光源之间的深度值
    float currentDepth = length(frag2light);
    float shadow = 0;
    float bias = max(0.05 * (1 - dot(normal, lightDir)), 0.005);
    // 根据观察者的距离来增加偏移半径，当距离更远的时候阴影更柔和，更近了就更锐利
    float diskRadius = (1.0 + (currentDepth / z_far)) / 25.0;
    // 对fragment位置周围过滤多个样本，并对结果平均化
    for (int i=0; i<20; i++)
    {
        // 获取立方体贴图中存储的深度
        // 计算fragment的位置与光的位置之间的差向量，使用这个向量作为一个方向向量去对立方体贴图进行采样
        float cloestDepth = texture(cube_map_shadow[light_idx], frag2light + sampleOffsetDirections[i] * diskRadius).r;
        // closest_depth在0到1的范围，所以我们先将其转换回0到z_far的范围
        cloestDepth *= z_far;
        shadow += currentDepth - bias > cloestDepth ? 1.0: 0.0;
    }
    return shadow / 20.0;
}

vec3 CalcLight(int light_idx, vec3 normal, vec3 viewDir, vec2 texcoords)
{
    Light light = lights[light_idx];
    vec3 lightDir = TangentLightDir[light_idx];
    // 聚光光源计算强度
    // 内圆锥之内强度为1，内圆锥到外圆锥之间强度由1减小到0
    float spot_intensity = 1;
    if (light.type == 2)
    {
        // theta = cos(光照方向, 光源到片段之间的方向)
        float theta = dot(FragPos - light.position, normalize(light.direction));
        float epsilon = light.innerCutOff - light.outerCutOff;
        spot_intensity = clamp((theta - light.outerCutOff) / epsilon, 0, 1);
    }

    // 计算光照衰减
    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);

    // 环境光照
    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse0, texcoords));

    // 漫反射光照：使物体上法线方向与光线方向越接近的片段能从光源处获得更多的亮度
    // 用点乘表示法线方向与光线方向的接近程度：两个单位向量的夹角越小，它们点乘的结果越倾向于1
    // 如果两个向量之间的角度大于90度，点乘的结果就会变成负数，这样会导致漫反射分量变为负数
    // 一般来说，物体表面只反射照在正面的光线，因此排除负数的情况
    float diffuse_param = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diffuse_param * light.diffuse * vec3(texture(material.texture_diffuse0, texcoords));

    // 镜面光照：当观察方向与反射方向接近时，可以看到高光
    float specular_param = 0;
    if (use_blinn > 0)
    {
        // 计算半程向量
        vec3 halfwayDir = normalize(lightDir + viewDir);
        // 然后计算半程向量与法向量的角度差，它们之间夹角越小，镜面光的作用就越大, 用点乘表示角度差
        specular_param = pow(max(dot(halfwayDir, normal), 0.0), material.shininess);
    }
    else
    {
        // 先用法向量翻折入射光的方向来计算反射向量。
        // reflect函数要求第一个向量是从光源指向片段位置的向量
        vec3 reflectDir = reflect(-lightDir, normal);
        // 然后计算反射向量与观察方向的角度差，它们之间夹角越小，镜面光的作用就越大, 用点乘表示角度差
        specular_param = pow(max(dot(reflectDir, viewDir), 0.0), material.shininess);
    }
    vec3 specular = specular_param * light.specular * vec3(texture(material.texture_specular0, texcoords));

    //vec3 emission = vec3(texture(material.emission, texcoords));
    // 计算阴影
    float shadow = CalcShadow(light_idx, lightDir, light.position, normal);
    
    return attenuation * (ambient + spot_intensity * (1 - shadow) * (diffuse + specular));// + emission;
}

vec4 CalcEnvironmentReflection(vec3 normal, vec3 viewDir, vec2 texcoords)
{
    // 计算由观察方向(摄像机->片段)关于法线的反射方向
    vec3 reflectDir = reflect(viewDir, normal);
    // 用反射方向在立方体贴图上采样
    return texture(material.texture_ambient0, texcoords) * texture(cube_map, reflectDir);
    //return texture(cube_map, reflectDir);
}

vec4 CalcEnvironmentRefraction(vec3 normal, vec3 viewDir)
{
    vec3 refractDir = refract(viewDir, normal, material.refract_ratio);
    return texture(cube_map, refractDir);
}

// 使用视差贴图对TexCoord进行偏移
vec2 ParallaxMapping(vec2 texcoords, vec3 viewDir)
{
    float layerNum = 30;
    // 一层的深度
    float layerDepth = 1.0 / layerNum;
    // TexCoord偏移一层的偏移值
    vec2 deltaTexcoords = viewDir.xy / viewDir.z * parallax_height_scale / layerNum;
    // 当前层所在的深度
    float currentLayerDepth = 0.0;
    // 当前uv
    vec2 currentTexcoords = texcoords;
    // 当前uv在视差贴图中采样的深度
    float currentMapDepth = texture(material.texture_parallax0, currentTexcoords).r;

    // 迭代中逐层对texcoords进行偏移
    while (currentMapDepth > currentLayerDepth)
    {
        currentLayerDepth += layerDepth;
        currentTexcoords -= deltaTexcoords;
        currentMapDepth = texture(material.texture_parallax0, currentTexcoords).r;
    }
    //return currentTexcoords;

    vec2 lastTexcoords = currentTexcoords + deltaTexcoords;
    float lastMapDepth = texture(material.texture_parallax0, lastTexcoords).r;
    float lastLayerDepth = currentLayerDepth - layerDepth;

    float d1 = currentMapDepth - currentLayerDepth;
    float d2 = lastLayerDepth - lastMapDepth;
    float weight = d1 / (d1 + d2);
    return mix(lastTexcoords, currentTexcoords, weight);
}

void main()
{   
    // 观察方向(片段->摄像机)
    vec3 TangentViewDir = normalize(TangentViewPos - TangentFragPos);
    
    vec2 mapping_texcoords = ParallaxMapping(TexCoord, TangentViewDir);

    // uv偏移之后可能会超出0~1的范围，当它超出默认纹理坐标范围的时候就丢弃
    if (mapping_texcoords.x < 0 || mapping_texcoords.x > 1 || mapping_texcoords.y < 0 || mapping_texcoords.y > 1)
        discard;

    // 从法线贴图采样获取法线
    vec3 TangentNorm = texture(material.texture_normal0, mapping_texcoords).rgb;
    // 将法线向量转换为范围[-1,1]
    TangentNorm = normalize(TangentNorm * 2.0 - 1.0);

    vec3 result = vec3(0);

    for (int i=0; i<light_num; i++)
        result += CalcLight(i, TangentNorm, TangentViewDir, mapping_texcoords);
    
    if (use_cube_map == 1)
    {
        result += vec3(CalcEnvironmentReflection(TangentNorm, -TangentViewDir, mapping_texcoords));
        //result += vec3(CalcEnvironmentRefraction(norm, -viewDir));
    }
    FragColor = vec4(result, 1.0f);
} 