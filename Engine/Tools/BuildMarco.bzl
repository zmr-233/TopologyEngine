# BuildMarco.bzl
load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

def engine_lib(name, srcs=[], hdrs=[], deps=[], visibility=None, extra_copts=[]):
    """游戏引擎中常用的 cc_library，默认启用C++23、-O3等"""
    copts = [
        "-std=c++23",
        "-O3",
        # 也可在此加入其他常用宏定义，比如 -DENGINE_BUILD=1
    ] + extra_copts
    cc_library(
        name = name,
        srcs = srcs,
        hdrs = hdrs,
        deps = deps,
        visibility = visibility if visibility else ["//visibility:public"],
        copts = copts,
        strip_include_prefix = "Public",
        include_prefix = "",
    )
    # 移除文件路径中的 "Public/"，使得编译器看到的路径变为 "TypeUtils/...".
    # include_prefix 设为 ""，意思是不再加额外前缀

def engine_test(name, srcs=[], deps=[], visibility=None, extra_copts=[]):
    """引擎内的测试宏"""
    copts = [
        "-std=c++23",
        "-O3",
    ] + extra_copts
    cc_test(
        name = name,
        srcs = srcs,
        deps = deps,
        visibility = visibility if visibility else ["//visibility:private"],
        copts = copts,
    )

# def editor_lib(...):
#     """若有专门 Editor 模块，可以再封装一层"""
#     # 类似写法，或者定制 Editor 特有编译宏
#     pass

# def plugin_lib(...):
#     """若支持插件化，也可写类似封装"""
#     pass
