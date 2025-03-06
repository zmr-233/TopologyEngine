#version 330 core


in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform vec3 viewPos; // 观察者位置


// 材质属性
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
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
    vec3 ambient = light.ambient * material.ambient;

    // 计算漫反射光照
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);

    //计算镜面光照
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess * 128.0);
    vec3 specular = light.specular * (spec * material.specular);

    // 计算结果颜色
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}