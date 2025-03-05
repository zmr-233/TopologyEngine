#version 330 core

in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    // 计算漫反射光照
    vec3 diffuse = diff * lightColor;
    // 计算环境光照
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * lightColor;

    // 计算结果颜色
    vec3 result = (ambient + diffuse) * objectColor;
    FragColor = vec4(result, 1.0);
}