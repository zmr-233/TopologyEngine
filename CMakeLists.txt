cmake_minimum_required(VERSION 3.10)
project(LearnGL LANGUAGES CXX)

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options(/Ox /arch:AVX2 /openmp)
else()
    add_compile_options(-O3 -march=native -flto -fopenmp)
endif()

# 如果使用的是vcpkg工具链文件，确保在命令行中设置：
# cmake -DCMAKE_TOOLCHAIN_FILE=[vcpkg root]/scripts/buildsystems/vcpkg.cmake ..

# 头文件目录
# 添加 Engine/Include 目录及其所有子目录
file(GLOB_RECURSE ALL_INCLUDE_DIRS LIST_DIRECTORIES true "${CMAKE_SOURCE_DIR}/Engine/Include")
foreach(include_dir ${ALL_INCLUDE_DIRS})
    if(IS_DIRECTORY "${include_dir}")
        include_directories("${include_dir}")
    endif()
endforeach()

# 添加源文件
file(GLOB_RECURSE SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/Engine/Source/*.cpp"
)

# 正确查找所有需要的包
find_package(nameof CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(TBB CONFIG REQUIRED)
find_package(Taskflow CONFIG REQUIRED)
find_package(OpenMP REQUIRED)

add_executable(${PROJECT_NAME} ${SOURCES})

# 正确链接所有库，包括glad
target_link_libraries(${PROJECT_NAME} PRIVATE 
    nameof::nameof
    glfw 
    GLEW::GLEW 
    glm::glm
    OpenGL::GL
    imgui::imgui
    glad::glad
    TBB::tbb 
    TBB::tbbmalloc
    Taskflow::Taskflow
    OpenMP::OpenMP_CXX
)

# 将整个 res 文件夹复制到构建目录
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/Resources
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/Resources
)