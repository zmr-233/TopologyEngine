cmake_minimum_required(VERSION 3.10)


set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 如果你使用的是vcpkg工具链文件，确保在命令行中设置它
# 例如: cmake -DCMAKE_TOOLCHAIN_FILE=[vcpkg root]/scripts/buildsystems/vcpkg.cmake ..

# 查找GLFW和GLEW包
find_package(glfw3 CONFIG REQUIRED)
find_package(GLEW REQUIRED)
# 查找GLM包
find_package(glm CONFIG REQUIRED)

# 添加stb_image.h的路径
include_directories(${CMAKE_SOURCE_DIR}/stb_image)

# 添加你的源文件
set(SOURCES src/main.cpp)
add_executable(${PROJECT_NAME} ${SOURCES})

# 链接GLFW、GLEW和GLM库
target_link_libraries(${PROJECT_NAME} PRIVATE glfw GLEW::GLEW glm)

# 如果你的项目使用OpenGL，你可能还需要链接OpenGL库
find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)

find_package(imgui CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE imgui::imgui)

# 将整个 res 文件夹复制到构建目录
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/res
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/res
)