cc_library(
    name = "learngl_lib",
    srcs = ["lib.cpp"],
    hdrs = ["lib.hpp"],
    # 引用引擎的 TypeUtilsLib
    copts = [
        "-std=c++23",
        "-O3",
    ],
    deps = [
        "@engine//Runtime/Core:TypeUtilsLib",
    ],
)

cc_binary(
    name = "learngl_app",
    srcs = ["main.cpp"],
    copts = [
        "-std=c++23",
        "-O3",
    ],
    deps = [
        ":learngl_lib",
    ],
)
