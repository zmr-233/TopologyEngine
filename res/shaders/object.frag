#version 330 core

in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos; // 光源位置
uniform vec3 viewPos; // 观察者位置
/* ^^^^^^^^ 通常来说光照计算在观察空间进行，因为(0, 0, 0)无成本的观察者位置
但是此处仍然使用世界空间，意味着要把摄像机的位置向量传递给片段着色器 */
void main()
{
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);

    // 计算漫反射光照
    vec3 diffuse = diff * lightColor;

    // 计算环境光照
    float ambientStrength = 0.05;
    vec3 ambient = ambientStrength * lightColor;

    //计算镜面光照
    float specularStrength = 0.5; //镜面强度(Specular Intensity)
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    /*^^^^^^reflect函数要求第一个向量是从光源指向片段位置的向量因此需要取反 */
    float shininess = 32; // 反光度
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * lightColor;

    // 计算结果颜色
    vec3 result = (ambient + diffuse+specular) * objectColor;
    FragColor = vec4(result, 1.0);
}