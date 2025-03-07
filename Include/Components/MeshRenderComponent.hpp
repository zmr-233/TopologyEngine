/****************************************
 * File: Include/Components/MeshRenderComponent.hpp
 * 说明：
 ****************************************/
#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#include <vector>

// 这里示例存储 VAO/VBO/EBO 等GL资源ID，以及纹理ID等
// 你可以根据自己需求再扩展，比如存储顶点数据指针、材质等
struct MeshRenderComponent {
    // OpenGL资源
    std::uint32_t VAO = 0;
    std::uint32_t VBO = 0;
    std::uint32_t EBO = 0;

    // 是否使用索引
    bool useIndex           = false;
    std::size_t indexCount  = 0;
    std::size_t vertexCount = 0;

    // 纹理IDs（diffuse, specular, normal, ...）
    std::vector<std::uint32_t> textures;
    // 如果没有纹理，可以用颜色代替
    glm::vec3 color{1.0f, 1.0f, 1.0f};

    // 也可以在这里放着色器ID，或者只放一个名字，在System里找对应的Shader
    std::uint32_t shaderID = 0;

    // ... 其他可能的材质信息(如shininess)
    float shininess = 0.5f;  // 128.0f * shininess

    //是否启用光照
    bool useLight = true;
};
