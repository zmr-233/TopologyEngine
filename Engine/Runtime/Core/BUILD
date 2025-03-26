load("@engine//Tools:BuildMarco.bzl", "engine_lib", "engine_test")

engine_lib(
    name = "MarcoUtilsLib",
    srcs = [],
    hdrs = glob(["Public/MarcoUtils/*.hpp"]),
)

engine_lib(
    name = "TypeUtilsLib",
    srcs = [],
    hdrs = glob(["Public/TypeUtils/*.hpp"]),
    deps = [
        ":MarcoUtilsLib",
    ],
)

# engine_lib(
#     name = "DebugUtilsLib",
#     srcs = glob(
#         ["Private/DebugUtils/*.cpp"],
#         allow_empty = True,
#     ),
#     hdrs = glob(["Public/DebugUtils/*.hpp"]),
#     deps = [
#         ":TypeUtilsLib",
#     ],
# )

# engine_lib(
#     name = "MemoryLib",
#     srcs = glob(
#         ["Private/Memory/*.cpp"],
#         allow_empty = True,
#     ),
#     hdrs = glob(["Public/Memory/*.hpp"]),
#     deps = [
#         ":TypeUtilsLib",
#     ],
# )

engine_lib(
    name = "ThreadLib",
    srcs = select(
        {
            "@platforms//os:linux": glob(
                ["Private/Thread/*.cpp"],
                exclude = ["Private/Thread/*_win.cpp"],
            ),
            "@platforms//os:windows": glob(
                ["Private/Thread/*.cpp"],
                exclude = ["Private/Thread/*_linux.cpp"],
            ),
            "//conditions:default": [],
        },
    ),
    hdrs = glob(["Public/Thread/*.hpp"]),
    copts = select(
        {
            "@platforms//os:windows": ["/EHsc"],  # 在 MSVC 下启用 C++ 异常处理
            "//conditions:default": [],
        },
    ),
    linkopts = select(
        {
            "@platforms//os:linux": ["-lpthread"],
            "//conditions:default": [],
        },
    ),
    deps = [
        ":TypeUtilsLib",
    ],
)

engine_lib(
    name = "TasksLib",
    srcs = glob(
        ["Private/Tasks/*.cpp"],
        allow_empty = True,
    ),
    hdrs = glob(["Public/Tasks/*.hpp"]),
    deps = [
        # ":MemoryLib",
        ":TypeUtilsLib",
    ],
)

# bazel test @engine//Runtime/Core:TypeUtilsTest
engine_test(
    name = "TypeUtilsTest",
    srcs = glob(["Tests/TypeUtilsTests/*.cpp"]),
    deps = [
        ":TypeUtilsLib",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)

# bazel test @engine//Runtime/Core:ThreadTest
engine_test(
    name = "ThreadTest",
    srcs = glob(["Tests/ThreadTests/*.cpp"]),
    deps = [
        ":ThreadLib",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)

# bazel test @engine//Runtime/Core:TasksTest
engine_test(
    name = "TasksTest",
    srcs = glob(["Tests/TasksTests/*.cpp"]),
    deps = [
        ":TasksLib",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)
