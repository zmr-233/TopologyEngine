cmake_minimum_required(VERSION 3.10)

# 主引擎=======================================================
project(ZmrEngine LANGUAGES CXX)

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

enable_testing()  # 开启测试功能

# 如果使用vcpkg：
# cmake -DCMAKE_TOOLCHAIN_FILE=[vcpkg root]/scripts/buildsystems/vcpkg.cmake ..

# 1) 收集引擎源文件
file(GLOB_RECURSE ENGINE_SOURCES
    "${CMAKE_SOURCE_DIR}/Engine/Source/*.cpp"
)
file(GLOB_RECURSE ENGINE_HEADERS
    "${CMAKE_SOURCE_DIR}/Engine/Include/*.h"
    "${CMAKE_SOURCE_DIR}/Engine/Include/*.hpp"
)

# 2) 创建静态库
add_library(EngineLib STATIC ${ENGINE_SOURCES} ${ENGINE_HEADERS})
file(GLOB_RECURSE ALL_INCLUDE_DIRS LIST_DIRECTORIES true "${CMAKE_SOURCE_DIR}/Engine/Include")
foreach(include_dir ${ALL_INCLUDE_DIRS})
    if(IS_DIRECTORY "${include_dir}")
        include_directories("${include_dir}")
    endif()
endforeach()
# 这使得 链接到 EngineLib 的目标也会继承这个 Include 路径
target_include_directories(EngineLib PUBLIC "${CMAKE_SOURCE_DIR}/Engine/Include")

# 链接三方库
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

target_link_libraries(EngineLib PRIVATE
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

# 设置编译优化选项
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(EngineLib PRIVATE /Ox /arch:AVX2 /openmp)
else()
    target_compile_options(EngineLib PRIVATE -O3 -march=native -flto -fopenmp)
endif()

# 复制资源
add_custom_command(TARGET EngineLib POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/Resources
        $<TARGET_FILE_DIR:EngineLib>/Resources
)

# 主程序=======================================================

# 主程序可执行文件
file(GLOB_RECURSE MAIN_SOURCE
    # 例如你想编译的主函数文件
    "${CMAKE_SOURCE_DIR}/Projects/EngineMain.cpp"
)
add_executable(EngineMain ${MAIN_SOURCE})
target_link_libraries(EngineMain PRIVATE 
    EngineLib
)


# 测试目标=======================================================

# 查找 Google Test
find_package(GTest CONFIG REQUIRED)

# 收集测试文件
file(GLOB_RECURSE ENGINE_TEST_SOURCES
    "${CMAKE_SOURCE_DIR}/Engine/Tests/*.cpp"
)

add_executable(EngineTests ${ENGINE_TEST_SOURCES})
target_link_libraries(EngineTests PRIVATE
    EngineLib   # 链接刚才的引擎库
    GTest::gtest
    GTest::gtest_main
    GTest::gmock
    GTest::gmock_main
)

# 为测试可执行文件添加测试命令
# 可以根据需要起多个测试名字，也可以只起一个
add_test(NAME EngineAllTests COMMAND EngineTests)
