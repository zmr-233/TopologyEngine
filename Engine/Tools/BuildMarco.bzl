"""
Engine build macros for C++ libraries and tests.

This module provides simplified interfaces for creating engine libraries and tests with
standardized compilation settings and include path handling across platforms.
"""

load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")

##############################################
# 1. 内部函数：_engine_cc_library_impl
#    最底层封装，用于构建 cc_library
##############################################
def _engine_cc_library_impl(
        name,
        srcs = [],
        hdrs = [],
        deps = [],
        visibility = None,
        # 通用编译选项和链接选项
        copts = [],
        linkopts = [],
        # 在外层宏传进来的包含目录（相对路径或绝对路径皆可）
        include_dirs = [],
        # 设置 strip_include_prefix / include_prefix 等高级属性
        strip_include_prefix = None,
        include_prefix = None,
        # 兼容平台属性
        target_compatible_with = []):
    # 最底层的 cc_library 封装，带有对 include_dirs 的处理。
    # 外部宏（engine_lib/engine_plib）通常不会直接用到它。

    # 在这里统一添加基础的编译选项
    base_copts = [
        "-std=c++23",
        "-O3",
    ]

    # 将外部传进来的 include_dirs 处理为 -Ixxx 的形式
    for inc in include_dirs:
        base_copts.append("-I" + inc)

    # 合并最终的 copts
    final_copts = base_copts + copts

    cc_library(
        name = name,
        srcs = srcs,
        hdrs = hdrs,
        deps = deps,
        visibility = visibility if visibility else ["//visibility:public"],
        copts = final_copts,
        linkopts = linkopts,
        target_compatible_with = target_compatible_with,

        # 如果我们希望使用 strip_include_prefix 和 include_prefix
        # 来简化头文件包含，则在此处设置
        strip_include_prefix = strip_include_prefix,
        include_prefix = include_prefix,
    )

##############################################
# 2. 内部函数：_engine_cc_test_impl
#    最底层封装，用于构建 cc_test
##############################################
def _engine_cc_test_impl(
        name,
        srcs = [],
        deps = [],
        visibility = None,
        copts = [],
        linkopts = [],
        include_dirs = [],
        target_compatible_with = []):
    # 最底层的 cc_test 封装，带有对 include_dirs 的处理。
    base_copts = [
        "-std=c++23",
        "-O3",
    ]
    for inc in include_dirs:
        base_copts.append("-I" + inc)
    final_copts = base_copts + copts

    cc_test(
        name = name,
        srcs = srcs,
        deps = deps,
        visibility = visibility if visibility else ["//visibility:private"],
        copts = final_copts,
        linkopts = linkopts,
        target_compatible_with = target_compatible_with,
    )

##############################################
# 3. 对外宏：engine_lib
#    一般的引擎库，不需要特殊的平台逻辑
##############################################
def engine_lib(
        name,
        srcs = [],
        hdrs = [],
        deps = [],
        visibility = None,
        # 给外部使用者设定的参数
        copts = [],
        linkopts = [],
        include_dirs = [],
        target_compatible_with = [],
        # 下面两个参数可根据需求是否暴露给调用者
        strip_include_prefix = "Public",
        include_prefix = ""):
    """常规引擎库。

    默认 strip_include_prefix="Public"、include_prefix=""，
    使得头文件可以直接 #include "TypeUtils/Invoke.hpp"
    而不需要包含完整路径。
    """
    _engine_cc_library_impl(
        name = name,
        srcs = srcs,
        hdrs = hdrs,
        deps = deps,
        visibility = visibility,
        copts = copts,
        linkopts = linkopts,
        include_dirs = include_dirs,
        target_compatible_with = target_compatible_with,
        strip_include_prefix = strip_include_prefix,
        include_prefix = include_prefix,
    )

##############################################
# 4. 对外宏：engine_test
#    构建测试目标
##############################################
def engine_test(
        name,
        srcs = [],
        deps = [],
        visibility = None,
        copts = [],
        linkopts = [],
        include_dirs = [],
        target_compatible_with = []):
    """构建引擎测试目标。

    如有私有测试头文件目录。
    可通过 include_dirs = ["Runtime/Core/Tests/TypeUtilsTests"] 的方式传入。
    """
    _engine_cc_test_impl(
        name = name,
        srcs = srcs,
        deps = deps,
        visibility = visibility,
        copts = copts,
        linkopts = linkopts,
        include_dirs = include_dirs,
        target_compatible_with = target_compatible_with,
    )

##############################################
# 5. 对外宏：engine_plib
#    用于多平台差异化的库目标
##############################################
def engine_plib(
        name,
        # 这里可以直接传 select(...) 后的 srcs/hdrs
        # 也可以让用户传 srcs_linux / srcs_windows 等自己选择封装方式
        srcs = [],
        hdrs = [],
        deps = [],
        visibility = None,
        # 传入平台相关的 copts、linkopts
        platform_copts = {},
        platform_linkopts = {},
        # 通用的 include_dirs
        include_dirs = [],
        target_compatible_with = [],
        strip_include_prefix = "Public",
        include_prefix = ""):
    """构建平台相关的库目标。

    用于创建能够适应不同平台(Linux/Windows)的C++库，
    根据当前平台自动选择适当的编译和链接选项。

    Args:
        name: 目标的名称
        srcs: 源代码文件列表，可以是select语句
        hdrs: 头文件列表，可以是select语句
        deps: 依赖项列表
        visibility: 可见性设置
        platform_copts: 平台特定的编译选项字典，如{"linux": [...], "windows": [...]}
        platform_linkopts: 平台特定的链接选项字典，如{"linux": [...], "windows": [...]}
        include_dirs: 包含目录列表
        target_compatible_with: 目标兼容性约束
        strip_include_prefix: 移除的头文件路径前缀
        include_prefix: 添加的头文件路径前缀
    """
    # 平台相关的库，通过 select 来判断 linux/windows，再决定追加的选项。

    # 设置一个 base_copts/base_linkopts，如果有需要
    base_copts = []
    base_linkopts = []

    # 让 copts 变成 select 结构，根据平台附加
    merged_copts = select({
        "@platforms//os:linux": base_copts + platform_copts.get("linux", []),
        "@platforms//os:windows": base_copts + platform_copts.get("windows", []),
        "//conditions:default": base_copts,
    })

    merged_linkopts = select({
        "@platforms//os:linux": base_linkopts + platform_linkopts.get("linux", []),
        "@platforms//os:windows": base_linkopts + platform_linkopts.get("windows", []),
        "//conditions:default": base_linkopts,
    })

    # 现在将以上结果传给底层的 _engine_cc_library_impl
    _engine_cc_library_impl(
        name = name,
        srcs = srcs,
        hdrs = hdrs,
        deps = deps,
        visibility = visibility,
        copts = merged_copts,
        linkopts = merged_linkopts,
        include_dirs = include_dirs,
        target_compatible_with = target_compatible_with,
        strip_include_prefix = strip_include_prefix,
        include_prefix = include_prefix,
    )
