/******************************************************
 * File: Resources/Shaders/light.frag
 ******************************************************/

#version 330 core
out vec4 FragColor;

// 相机位置
uniform vec3 viewPos; //为了统一参数，这里也加上了viewPos
uniform vec3 color;

void main()
{
    FragColor = vec4(1.0) * vec4(color, 1.0); // 将向量的四个分量全部设置为1.0
}