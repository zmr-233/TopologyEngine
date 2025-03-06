#ifndef MATERIAL_H
#define MATERIAL_H

#include <functional>
#include <map>
#include <string>

#include "shader.hpp"

inline std::map<std::string, std::function<void(Shader&)>> materials = {
    {"emerald",  // 祖母绿，一种绿色宝石
     [](Shader& context) {
         context.setFloat("material.ambient", {0.0215f, 0.1745f, 0.0215f});
         context.setFloat("material.diffuse", {0.07568f, 0.61424f, 0.07568f});
         context.setFloat("material.specular", {0.633f, 0.727811f, 0.633f});
         context.setFloat("material.shininess", 0.6f);
     }},
    {"jade",  // 玉，常见于东方文化的宝石
     [](Shader& context) {
         context.setFloat("material.ambient", {0.135f, 0.2225f, 0.1575f});
         context.setFloat("material.diffuse", {0.54f, 0.89f, 0.63f});
         context.setFloat("material.specular", {0.316228f, 0.316228f, 0.316228f});
         context.setFloat("material.shininess", 0.1f);
     }},
    {"obsidian",  // 黑曜石，火山玻璃
     [](Shader& context) {
         context.setFloat("material.ambient", {0.05375f, 0.05f, 0.06625f});
         context.setFloat("material.diffuse", {0.18275f, 0.17f, 0.22525f});
         context.setFloat("material.specular", {0.332741f, 0.328634f, 0.346435f});
         context.setFloat("material.shininess", 0.3f);
     }},
    {"pearl",  // 珍珠，由贝类分泌形成
     [](Shader& context) {
         context.setFloat("material.ambient", {0.25f, 0.20725f, 0.20725f});
         context.setFloat("material.diffuse", {1.0f, 0.829f, 0.829f});
         context.setFloat("material.specular", {0.296648f, 0.296648f, 0.296648f});
         context.setFloat("material.shininess", 0.088f);
     }},
    {"ruby",  // 红宝石
     [](Shader& context) {
         context.setFloat("material.ambient", {0.1745f, 0.01175f, 0.01175f});
         context.setFloat("material.diffuse", {0.61424f, 0.04136f, 0.04136f});
         context.setFloat("material.specular", {0.727811f, 0.626959f, 0.626959f});
         context.setFloat("material.shininess", 0.6f);
     }},
    {"turquoise",  // 绿松石
     [](Shader& context) {
         context.setFloat("material.ambient", {0.1f, 0.18725f, 0.1745f});
         context.setFloat("material.diffuse", {0.396f, 0.74151f, 0.69102f});
         context.setFloat("material.specular", {0.297254f, 0.30829f, 0.306678f});
         context.setFloat("material.shininess", 0.1f);
     }},
    {"brass",  // 黄铜
     [](Shader& context) {
         context.setFloat("material.ambient", {0.329412f, 0.223529f, 0.027451f});
         context.setFloat("material.diffuse", {0.780392f, 0.568627f, 0.113725f});
         context.setFloat("material.specular", {0.992157f, 0.941176f, 0.807843f});
         context.setFloat("material.shininess", 0.21794872f);
     }},
    {"bronze",  // 青铜
     [](Shader& context) {
         context.setFloat("material.ambient", {0.2125f, 0.1275f, 0.054f});
         context.setFloat("material.diffuse", {0.714f, 0.4284f, 0.18144f});
         context.setFloat("material.specular", {0.393548f, 0.271906f, 0.166721f});
         context.setFloat("material.shininess", 0.2f);
     }},
    {"chrome",  // 铬（金属元素，常见电镀层）
     [](Shader& context) {
         context.setFloat("material.ambient", {0.25f, 0.25f, 0.25f});
         context.setFloat("material.diffuse", {0.4f, 0.4f, 0.4f});
         context.setFloat("material.specular", {0.774597f, 0.774597f, 0.774597f});
         context.setFloat("material.shininess", 0.6f);
     }},
    {"copper",  // 铜
     [](Shader& context) {
         context.setFloat("material.ambient", {0.19125f, 0.0735f, 0.0225f});
         context.setFloat("material.diffuse", {0.7038f, 0.27048f, 0.0828f});
         context.setFloat("material.specular", {0.256777f, 0.137622f, 0.086014f});
         context.setFloat("material.shininess", 0.1f);
     }},
    {"gold",  // 黄金
     [](Shader& context) {
         context.setFloat("material.ambient", {0.24725f, 0.1995f, 0.0745f});
         context.setFloat("material.diffuse", {0.75164f, 0.60648f, 0.22648f});
         context.setFloat("material.specular", {0.628281f, 0.555802f, 0.366065f});
         context.setFloat("material.shininess", 0.4f);
     }},
    {"silver",  // 白银
     [](Shader& context) {
         context.setFloat("material.ambient", {0.19225f, 0.19225f, 0.19225f});
         context.setFloat("material.diffuse", {0.50754f, 0.50754f, 0.50754f});
         context.setFloat("material.specular", {0.508273f, 0.508273f, 0.508273f});
         context.setFloat("material.shininess", 0.4f);
     }},
    {"black plastic",
     [](Shader& context) {
         context.setFloat("material.ambient", {0.0f, 0.0f, 0.0f});
         context.setFloat("material.diffuse", {0.01f, 0.01f, 0.01f});
         context.setFloat("material.specular", {0.5f, 0.5f, 0.5f});
         context.setFloat("material.shininess", 0.25f);
     }},
    {"cyan plastic",
     [](Shader& context) {
         context.setFloat("material.ambient", {0.0f, 0.1f, 0.06f});
         context.setFloat("material.diffuse", {0.0f, 0.50980392f, 0.50980392f});
         context.setFloat("material.specular", {0.50196078f, 0.50196078f, 0.50196078f});
         context.setFloat("material.shininess", 0.25f);
     }},
    {"green plastic",
     [](Shader& context) {
         context.setFloat("material.ambient", {0.0f, 0.0f, 0.0f});
         context.setFloat("material.diffuse", {0.1f, 0.35f, 0.1f});
         context.setFloat("material.specular", {0.45f, 0.55f, 0.45f});
         context.setFloat("material.shininess", 0.25f);
     }},
    {"red plastic",
     [](Shader& context) {
         context.setFloat("material.ambient", {0.0f, 0.0f, 0.0f});
         context.setFloat("material.diffuse", {0.5f, 0.0f, 0.0f});
         context.setFloat("material.specular", {0.7f, 0.6f, 0.6f});
         context.setFloat("material.shininess", 0.25f);
     }},
    {"white plastic",
     [](Shader& context) {
         context.setFloat("material.ambient", {0.0f, 0.0f, 0.0f});
         context.setFloat("material.diffuse", {0.55f, 0.55f, 0.55f});
         context.setFloat("material.specular", {0.7f, 0.7f, 0.7f});
         context.setFloat("material.shininess", 0.25f);
     }},
    {"yellow plastic",
     [](Shader& context) {
         context.setFloat("material.ambient", {0.0f, 0.0f, 0.0f});
         context.setFloat("material.diffuse", {0.5f, 0.5f, 0.0f});
         context.setFloat("material.specular", {0.6f, 0.6f, 0.5f});
         context.setFloat("material.shininess", 0.25f);
     }},
    {"black rubber",
     [](Shader& context) {
         context.setFloat("material.ambient", {0.02f, 0.02f, 0.02f});
         context.setFloat("material.diffuse", {0.01f, 0.01f, 0.01f});
         context.setFloat("material.specular", {0.4f, 0.4f, 0.4f});
         context.setFloat("material.shininess", 0.078125f);
     }},
    {"cyan rubber",
     [](Shader& context) {
         context.setFloat("material.ambient", {0.0f, 0.05f, 0.05f});
         context.setFloat("material.diffuse", {0.4f, 0.5f, 0.5f});
         context.setFloat("material.specular", {0.04f, 0.7f, 0.7f});
         context.setFloat("material.shininess", 0.078125f);
     }},
    {"green rubber",
     [](Shader& context) {
         context.setFloat("material.ambient", {0.0f, 0.05f, 0.0f});
         context.setFloat("material.diffuse", {0.4f, 0.5f, 0.4f});
         context.setFloat("material.specular", {0.04f, 0.7f, 0.04f});
         context.setFloat("material.shininess", 0.078125f);
     }},
    {"red rubber",
     [](Shader& context) {
         context.setFloat("material.ambient", {0.05f, 0.0f, 0.0f});
         context.setFloat("material.diffuse", {0.5f, 0.4f, 0.4f});
         context.setFloat("material.specular", {0.7f, 0.04f, 0.04f});
         context.setFloat("material.shininess", 0.078125f);
     }},
    {"white rubber",
     [](Shader& context) {
         context.setFloat("material.ambient", {0.05f, 0.05f, 0.05f});
         context.setFloat("material.diffuse", {0.5f, 0.5f, 0.5f});
         context.setFloat("material.specular", {0.7f, 0.7f, 0.7f});
         context.setFloat("material.shininess", 0.078125f);
     }},
    {"yellow rubber",
     [](Shader& context) {
         context.setFloat("material.ambient", {0.05f, 0.05f, 0.0f});
         context.setFloat("material.diffuse", {0.5f, 0.5f, 0.4f});
         context.setFloat("material.specular", {0.7f, 0.7f, 0.04f});
         context.setFloat("material.shininess", 0.078125f);
     }}};

#endif