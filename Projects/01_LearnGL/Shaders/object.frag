/******************************************************
 * FILE: Resources/Shaders/object.frag
 ******************************************************/

#version 330 core

out vec4 FragColor;

// 顶点着色器传来的
in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

// 相机位置
uniform vec3 viewPos;

// 材质
struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};
uniform Material material;

// 光源信息
struct Light {
    int  type;         // 0=Directional, 1=Point, 2=Spot, 3=Flashlight
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

uniform int   numLights;
uniform Light lights[16]; // 你可以设大一些

void main()
{
    vec3 norm    = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // 最终累加的颜色
    vec3 result  = vec3(0.0);

    for(int i = 0; i < numLights; i++)
    {
        Light light = lights[i];

        // 先准备通用的几个量
        vec3 ambient  = light.ambient * texture(material.diffuse, TexCoords).rgb;

        // (A) 如果是 Directional，就用 -light.direction 当光线方向
        // (B) 如果是 Point / Spot / Flashlight, 就用 light.position - FragPos
        vec3 lightDir;
        if (light.type == 0) { // Directional
            lightDir = normalize(-light.direction);
        }
        else {
            lightDir = normalize(light.position - FragPos);
        }

        // 漫反射
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;

        // 镜面
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec      = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess * 128.0);
        vec3 specular   = light.specular * spec * texture(material.specular, TexCoords).rgb;

        // 衰减系数(默认=1)
        float attenuation = 1.0;
        // Directional光 不需要衰减。Point/Spot/Flashlight需要
        if (light.type != 0) {
            float distance = length(light.position - FragPos);
            attenuation = 1.0 / (light.constant + light.linear*distance + 
                                 light.quadratic*(distance*distance));
        }

        // Spot 或 Flashlight: 判断聚光角度
        float spotFactor = 1.0;
        if (light.type == 2 || light.type == 3) {
            float theta   = dot(lightDir, normalize(-light.direction));
            float epsilon = light.cutOff - light.outerCutOff;
            float inten   = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
            spotFactor    = inten;
        }

        // 累加
        diffuse  *= attenuation * spotFactor;
        specular *= attenuation * spotFactor;

        // 最终
        result += ambient + diffuse + specular;
    }

    FragColor = vec4(result, 1.0);
}
