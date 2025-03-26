load("@engine//Tools:BuildMarco.bzl", "engine_lib", "engine_plib", "engine_test")

##############################################
# 1. 常规库：MarcoUtilsLib
##############################################
engine_lib(
    name = "MarcoUtilsLib",
    srcs = [],
    hdrs = glob(["Public/MarcoUtils/*.hpp"]),
    include_dirs = [
        "Engine/Runtime/Core/Public",
    ],
)

##############################################
# 2. 常规库：TypeUtilsLib
##############################################
engine_lib(
    name = "TypeUtilsLib",
    srcs = [],
    hdrs = glob(["Public/TypeUtils/*.hpp"]),
    include_dirs = [
        "Engine/Runtime/Core/Public",
    ],
    deps = [":MarcoUtilsLib"],
)

##############################################
# 3. 跨平台库：ThreadLib
##############################################
engine_plib(
    name = "ThreadLib",
    srcs = select({
        "@platforms//os:linux": glob(
            ["Private/Thread/*.cpp"],
            exclude = ["Private/Thread/*_win.cpp"],
        ),
        "@platforms//os:windows": glob(
            ["Private/Thread/*.cpp"],
            exclude = ["Private/Thread/*_linux.cpp"],
        ),
        "//conditions:default": [],
    }),
    hdrs = glob(["Public/Thread/*.hpp"]),
    include_dirs = [
        "Engine/Runtime/Core/Public",
    ],
    platform_copts = {
        # Windows: 启用异常处理
        "windows": ["/EHsc"],
    },
    platform_linkopts = {
        # Linux: 链接 pthread
        "linux": ["-lpthread"],
    },
    deps = [
        ":TypeUtilsLib",
    ],
)

##############################################
# 4. 常规库：TasksLib
##############################################
engine_lib(
    name = "TasksLib",
    srcs = glob(
        ["Private/Tasks/*.cpp"],
        allow_empty = True,
    ),
    hdrs = glob(["Public/Tasks/*.hpp"]),
    include_dirs = [
        "Engine/Runtime/Core/Public",
    ],
    deps = [
        # ":MemoryLib",  # 如有需要可以加回来
        ":TypeUtilsLib",
    ],
)

##############################################
# 5. 测试：TypeUtilsTest
##############################################
engine_test(
    name = "TypeUtilsTest",
    srcs = glob(["Tests/TypeUtilsTests/*.cpp"]),
    include_dirs = [
        "Engine/Runtime/Core/Public",
        "Engine/Runtime/Core/Tests/TypeUtilsTests",
    ],
    deps = [
        ":TypeUtilsLib",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)

engine_test(
    name = "ThreadTest",
    srcs = glob(["Tests/ThreadTests/*.cpp"]),
    include_dirs = [
        "Engine/Runtime/Core/Public",
        "Engine/Runtime/Core/Tests/ThreadTests",
    ],
    deps = [
        ":ThreadLib",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)

engine_test(
    name = "TasksTest",
    srcs = glob(["Tests/TasksTests/*.cpp"]),
    include_dirs = [
        "Engine/Runtime/Core/Public",
        "Engine/Runtime/Core/Tests/TasksTests",
    ],
    deps = [
        ":TasksLib",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)
