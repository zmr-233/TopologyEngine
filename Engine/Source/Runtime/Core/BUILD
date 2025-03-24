load("@engine//Tools:BuildMarco.bzl", "engine_lib", "engine_test")

engine_lib(
    name = "TypeUtilsLib",
    srcs = [],
    hdrs = glob(["Public/TypeUtils/*.hpp"]),
)

# bazel test //Source/Runtime/Core:TypeUtilsTest
engine_test(
    name = "TypeUtilsTest",
    srcs = glob(["Tests/TypeUtilsTests/InvokeTest.cpp"]),
    deps = [
        ":TypeUtilsLib", 
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)