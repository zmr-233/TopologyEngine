#version 330 core


in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords; // 从顶点着色器传递纹理坐标

out vec4 FragColor;

uniform vec3 viewPos; // 观察者位置


// 材质属性
struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission; // 添加发光贴图
    float shininess;
}; 
uniform Material material;

// 光照属性
struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;


void main()
{
    // 计算环境光照
    // vec3 ambient = light.ambient * material.ambient;
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));

    // 计算漫反射光照
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    // vec3 diffuse = light.diffuse * (diff * material.diffuse);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));

    //计算镜面光照
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess * 128.0);
    vec3 specular = light.specular * (spec * vec3(texture(material.specular, TexCoords)));

    // 计算结果颜色
    vec3 result = ambient + diffuse + specular + vec3(texture(material.emission, TexCoords));
    FragColor = vec4(result, 1.0);
}