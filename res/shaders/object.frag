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
    float shininess;
}; 
uniform Material material;

// 光照属性
struct Light {
    vec3 position;
    vec3 direction;

    float cutOff; //聚光内圈
    float outerCutOff; //聚光外圈

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    //衰减常数
    float constant;
    float linear;
    float quadratic;
};
uniform Light light;


void main()
{   
    vec3 lightDir = normalize(light.position - FragPos);
    // vec3 lightDir = normalize(-light.direction); 

    // 计算环境光照
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
    
    //计算是否在光照范围
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    
    // 计算漫反射光照    
    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;
    
    //计算镜面光照
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess * 128.0);
    vec3 specular = light.specular * (spec * texture(material.specular, TexCoords).rgb);
    
    //计算点光源衰减
    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // 计算结果颜色
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);

}