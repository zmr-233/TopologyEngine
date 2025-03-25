load("@engine//Tools:BuildMarco.bzl", "engine_lib", "engine_test")
# package(default_visibility = ["//visibility:public"])engine_lib


engine_lib(
    name = "TypeUtilsLib",
    srcs = [],
    hdrs = glob(["Public/TypeUtils/*.hpp"]),
)

engine_lib(
    name="Thread_linux",
    srcs=glob(["Private/Thread/*_linux.cpp"]),
    hdrs=glob(["Public/Thread/*.hpp"]),
    target_compatible_with=[
        "@platforms//os:linux",
    ],
    linkopts=select(
        {
            "@platforms//os:linux": ["-lpthread"],
            "//conditions:default": [],
        }
    ),
)

engine_lib(
    name="Thread_win",
    srcs=glob(["Private/Thread/*_win.cpp"]),
    hdrs=glob(["Public/Thread/*.hpp"]),
    target_compatible_with=[
        "@platforms//os:windows",
    ],
    copts=select(
        {
            "@platforms//os:windows": ["/EHsc"],  # 在 MSVC 下启用 C++ 异常处理
            "//conditions:default": [],
        }
    ),
)

engine_lib(
    name = "ThreadLib",
    hdrs=glob(["Public/Thread/*.hpp"]),
    deps = select({
        "@platforms//os:linux": [":Thread_linux"],
        "@platforms//os:windows": [":Thread_win"],
        "//conditions:default": [],
    }),
)

# bazel test @engine//Source/Runtime/Core:TypeUtilsTest
engine_test(
    name = "TypeUtilsTest",
    srcs = glob(["Tests/TypeUtilsTests/InvokeTest.cpp"]),
    deps = [
        ":TypeUtilsLib", 
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)

# bazel test @engine//Source/Runtime/Core:ThreadTest
engine_test(
    name = "ThreadTest",
    srcs = glob(["Tests/ThreadTests/ThreadTest.cpp"]),
    deps = [
        ":ThreadLib",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)