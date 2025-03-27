##############################################
# 文件：Engine/Tools/cc_toolchain_config.bzl
##############################################

"""
Configuration for C++ toolchains in the build system.
This module provides custom toolchain configurations for Clang on Linux.
"""

# https://bazel.build/docs/cc-toolchain-config-reference?hl=zh-cn
load("@rules_cc//cc:action_names.bzl", "ACTION_NAMES")
load(
    "@rules_cc//cc:cc_toolchain_config_lib.bzl",
    "action_config",
    "feature",
    "flag_group",
    "flag_set",
    "tool",
    "tool_path",
)
load("@rules_cc//cc:defs.bzl", "CcToolchainConfigInfo", "cc_common")

all_compile_actions = [
    ACTION_NAMES.c_compile,
    ACTION_NAMES.cpp_compile,
    ACTION_NAMES.linkstamp_compile,
    ACTION_NAMES.assemble,
    ACTION_NAMES.preprocess_assemble,
    ACTION_NAMES.cpp_header_parsing,
    ACTION_NAMES.cpp_module_compile,
    ACTION_NAMES.cpp_module_codegen,
]

all_link_actions = [
    ACTION_NAMES.cpp_link_executable,
    ACTION_NAMES.cpp_link_dynamic_library,
    ACTION_NAMES.cpp_link_nodeps_dynamic_library,
    ACTION_NAMES.cpp_link_static_library,
]

def _make_linux_clang_toolchain_config_impl(ctx):
    """
    为 Linux x86_64 + Clang 构建一个 minimal 的 CcToolchainConfigInfo
    """

    # 1) 指定基础工具的路径
    #    - 注意，这里演示直接写死 /usr/bin/clang, /usr/bin/clang++ 等
    #    - 若你有更复杂的情况，可以改为 select() 或者 ctx.file
    llvm_version = "19"

    tool_paths = [
        tool_path(name = "gcc", path = "/usr/bin/clang"),  # Bazel 的内部名 "gcc" 其实就是编译器入口
        tool_path(name = "g++", path = "/usr/bin/clang++"),  # Bazel 内部名 "g++" 对应 C++ 编译入口
        tool_path(name = "ar", path = "/usr/bin/llvm-ar"),
        tool_path(name = "cpp", path = "/usr/bin/clang-cpp"),  # Bazel 内部名 "cpp" 对应 C++ 预处理器
        tool_path(name = "gcov", path = "/usr/bin/llvm-cov"),  # gcov 是 llvm-cov 的别名
        tool_path(name = "ld", path = "/usr/bin/clang++"),  # <=== 经过验证，这条完全没用，必须要强制指定每一个link action
        tool_path(name = "nm", path = "/usr/bin/llvm-nm"),
        tool_path(name = "objdump", path = "/usr/bin/llvm-objdump"),
        tool_path(name = "strip", path = "/usr/bin/llvm-strip"),
    ]

    # 2) 定义编译时的一些简单 flags
    #    - 下面演示一个“默认编译参数” feature
    default_compile_flags = feature(
        name = "default_compile_flags",
        enabled = True,
        flag_sets = [
            flag_set(
                actions = all_compile_actions,
                flag_groups = [
                    flag_group(
                        flags = [
                            "-fPIC",  # 位置无关代码
                            "-Wall",  # 启用所有常见警告
                            "-Wextra",  # 启用额外警告
                            "-Wpedantic",  # 严格标准检查: 警告任何不符合ISO C/C++标准的代码
                            "-fno-omit-frame-pointer",  #不省略帧指针: 便于调试和性能分析工具生成更准确的堆栈跟踪
                            "-std=c++23",
                            # "-stdlib=libc++",  # <--- add this
                        ],
                    ),
                ],
            ),
        ],
    )

    # 3) 定义 c++-link-executable 的 action_config
    # default_link_flags = feature(
    #     name = "default_link_flags",
    #     enabled = True,
    #     flag_sets = [
    #         flag_set(
    #             actions = all_link_actions,
    #             flag_groups = [
    #                 flag_group(
    #                     flags = [
    #                         # "-lc++",
    #                         # "-lc++abi",
    #                         # "-lm",
    #                         # "-ldl",
    #                     ],
    #                 ),
    #             ],
    #         ),
    #     ],
    # )

    link_exe = action_config(
        action_name = ACTION_NAMES.cpp_link_executable,
        enabled = True,
        tools = [
            tool(
                path = "/usr/bin/clang++",
            ),
        ],
        flag_sets = [],
    )

    link_dynamic_lib = action_config(
        action_name = ACTION_NAMES.cpp_link_dynamic_library,
        enabled = True,
        tools = [
            tool(
                path = "/usr/bin/clang++",
            ),
        ],
        flag_sets = [],
    )

    link_nodeps_dynamic_lib = action_config(
        action_name = ACTION_NAMES.cpp_link_nodeps_dynamic_library,
        enabled = True,
        tools = [
            tool(
                path = "/usr/bin/clang++",
            ),
        ],
        flag_sets = [],
    )

    link_static_lib = action_config(
        action_name = ACTION_NAMES.cpp_link_static_library,
        enabled = True,
        tools = [
            tool(
                path = "/usr/bin/clang++",
            ),
        ],
        flag_sets = [],
    )

    # 4) 组装并返回
    return cc_common.create_cc_toolchain_config_info(
        ctx = ctx,
        features = [
            default_compile_flags,
            # default_link_flags,
            # 根据需要可继续加 "supports_pic", "supports_dynamic_linker" 等
        ],
        action_configs = [
            link_exe,
            # link_dynamic_lib,
            # link_nodeps_dynamic_lib,
            # link_static_lib,
        ],
        # echo "" | clang++ -E -xc++ -v -
        cxx_builtin_include_directories = [
            "/usr/include/c++",
            # "/usr/include/c++/14.2.1",
            # "/usr/include/c++/14.2.1/x86_64-pc-linux-gnu",
            # "/usr/include/c++/14.2.1/backward",
            "/usr/lib/clang/{}/include".format(llvm_version),
            "/usr/local/include",
            "/usr/include",
        ],
        compiler = "clang",
        toolchain_identifier = "linux_clang_x86_64",
        tool_paths = tool_paths,
    )

linux_clang_x86_64_toolchain_config_gen = rule(
    implementation = _make_linux_clang_toolchain_config_impl,
    attrs = {},
    provides = [CcToolchainConfigInfo],
)

# def _make_windows_clang_toolchain_config_impl(ctx):
#     """为 Windows x86_64 + Clang-cl 构建一个 minimal 的 CcToolchainConfigInfo

#     假设在 C:/Program Files/LLVM/bin 下安装了 clang 工具
#     """

#     llvm_version = "14.0.6"
#     llvm_prefix = "C:/Program Files/llvm-{}".format(llvm_version)
#     llvm_bindir = llvm_prefix + "/bin"

#     # 1) 工具路径
#     tool_paths = [
#         tool_path(name = "gcc", path = llvm_bindir + "/clang-cl.exe"),
#         tool_path(name = "g++", path = llvm_bindir + "/clang-cl.exe"),
#         tool_path(name = "ar", path = llvm_bindir + "/llvm-lib.exe"),
#         tool_path(name = "ld", path = llvm_bindir + "/lld-link.exe"),
#         tool_path(name = "nm", path = llvm_bindir + "/llvm-nm.exe"),
#         tool_path(name = "objdump", path = llvm_bindir + "/llvm-objdump.exe"),
#         tool_path(name = "strip", path = llvm_bindir + "/llvm-strip.exe"),
#     ]

#     # 2) 默认编译 flags (MSVC-like)
#     default_compile_flags = feature(
#         name = "default_compile_flags",
#         enabled = True,
#         flag_sets = [
#             flag_set(
#                 actions = all_compile_actions,
#                 flag_groups = ([
#                     flag_group(
#                         flags = [
#                             # 注意在 clang-cl 下通常用 / 符号的参数风格
#                             "/Zc:__cplusplus",  # 让 __cplusplus 宏报告正确的值
#                             "/std:c++23",
#                             "/EHsc",  # 启用 C++ 异常处理
#                             "/O2",
#                         ],
#                     ),
#                 ]),
#             ),
#         ],
#     )

#     # 3) 链接器 action_config
#     default_link_flags = feature(
#         name = "default_link_flags",
#         enabled = True,
#         flag_sets = [
#             flag_set(
#                 actions = all_link_actions,
#                 flag_groups = [
#                     flag_group(
#                         flags = [
#                             # Windows 下用 clang-cl + lld-link
#                             "/LD",  # 如果要生成 dll 可以改 /LD
#                         ],
#                     ),
#                 ],
#             ),
#         ],
#     )

#     # link_exe = action_config(
#     #     action_name = "c++-link-executable",
#     #     enabled = True,
#     #     tools = [
#     #         tool(
#     #             path = llvm_bindir + "/clang-cl.exe",
#     #         ),
#     #     ],
#     #     flag_sets = [
#     #         flag_set(
#     #             actions = [
#     #                 "c++-link-executable",
#     #             ],
#     #             flag_groups = [
#     #                 flag_group(
#     #                     flags = [
#     #                         # Windows 下用 clang-cl + lld-link
#     #                         "/LD",  # 如果要生成 dll 可以改 /LD
#     #                     ],
#     #                 ),
#     #             ],
#     #         ),
#     #     ],
#     # )

#     return cc_common.create_cc_toolchain_config_info(
#         ctx = ctx,
#         tool_paths = tool_paths,
#         # action_configs = [
#         #     link_exe,
#         # ],
#         features = [
#             default_compile_flags,
#             default_link_flags,
#         ],
#         toolchain_identifier = "windows_clang_x86_64",
#     )

# windows_clang_x86_64_toolchain_config_gen = rule(
#     implementation = _make_windows_clang_toolchain_config_impl,
#     attrs = {},
#     provides = [CcToolchainConfigInfo],
# )
