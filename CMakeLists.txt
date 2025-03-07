cmake_minimum_required(VERSION 3.10)
project(LearnGL LANGUAGES CXX)

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 如果使用的是vcpkg工具链文件，确保在命令行中设置：
# cmake -DCMAKE_TOOLCHAIN_FILE=[vcpkg root]/scripts/buildsystems/vcpkg.cmake ..

# 头文件目录
include_directories(${CMAKE_SOURCE_DIR}/Include)

# 添加源文件
file(GLOB_RECURSE SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp"
)

# 正确查找所有需要的包
find_package(glfw3 CONFIG REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)

add_executable(${PROJECT_NAME} ${SOURCES})

# 正确链接所有库，包括glad
target_link_libraries(${PROJECT_NAME} PRIVATE 
    glfw 
    GLEW::GLEW 
    glm::glm
    OpenGL::GL
    imgui::imgui
    glad::glad  # 添加对glad库的链接
)

# 将整个 res 文件夹复制到构建目录
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/Resources
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/Resources
)