# Bazel 在加载一个 .bzl 文件时，需要该文件所在的目录（或其父目录）是一个合法的 “Bazel package”
# 而 “Bazel package” 的最低要求就是：该目录（或父目录）必须包含一个 BUILD（或 BUILD.bazel）文件——即便是空文件也行

#https://bazel.googlesource.com/rules_cc/+/refs/heads/master/examples/custom_toolchain?autodive=0%2F%2F%2F

load(
    "@engine//Tools:cc_toolchain_config.bzl",
    "linux_clang_x86_64_toolchain_config_gen",
)

# "windows_clang_x86_64_toolchain_config_gen",
load("@rules_cc//cc/toolchains:cc_toolchain.bzl", "cc_toolchain")

package(default_visibility = ["//visibility:public"])

filegroup(name = "empty")

# ############################
# # 1) Linux Clang cc_toolchain
# ############################

# 实例化 config rule -> 产出 CcToolchainConfigInfo
linux_clang_x86_64_toolchain_config_gen(
    name = "linux_clang_x86_64_toolchain_config",
)

# cc_toolchain_suite(
#     name = "clang_suite",
#     toolchains = {
#         "linux_x86_64": ":linux_clang_x86_64_toolchain",
#     },
# )

# https://bazel.build/reference/be/c-cpp?hl=zh-cn#cc_toolchain
cc_toolchain(
    name = "linux_clang_x86_64_toolchain",
    all_files = ":empty",
    ar_files = ":empty",
    compiler_files = ":empty",
    dwp_files = ":empty",
    linker_files = ":empty",
    objcopy_files = ":empty",
    strip_files = ":empty",
    # supports_param_files = 0,
    toolchain_config = ":linux_clang_x86_64_toolchain_config",
    # toolchain_identifier = "linux_clang_x86_64",
)

toolchain(
    name = "clang_suite",
    target_compatible_with = [
        "@platforms//cpu:x86_64",
    ],
    toolchain = ":linux_clang_x86_64_toolchain",
    toolchain_type = "@bazel_tools//tools/cpp:toolchain_type",
)

# toolchain(
#     name = "linux_clang_x86_64",
#     # Bazel 会根据当前平台 (和 --platforms 的设定) 匹配
#     # 这里声明：当 target 平台 = Linux + x86_64 时，就选用此 toolchain
#     exec_compatible_with = [
#         "@platforms//os:linux",
#         "@platforms//cpu:x86_64",
#     ],
#     target_compatible_with = [
#         "@platforms//os:linux",
#         "@platforms//cpu:x86_64",
#     ],
#     toolchain = ":linux_clang_x86_64_cc_toolchain",
#     toolchain_type = "@bazel_tools//tools/cpp:toolchain_type",
# )

# ############################
# # 2) Windows Clang cc_toolchain
# ############################

# windows_clang_x86_64_toolchain_config_gen(
#     name = "windows_clang_x86_64_toolchain_config",
# )

# cc_toolchain(
#     name = "windows_clang_x86_64_cc_toolchain",
#     all_files = ":empty",
#     compiler_files = ":empty",
#     dwp_files = ":empty",
#     linker_files = ":empty",
#     objcopy_files = ":empty",
#     strip_files = ":empty",
#     toolchain_config = ":windows_clang_x86_64_toolchain_config",
#     toolchain_identifier = "windows_clang_x86_64",
# )

# toolchain(
#     name = "windows_clang_x86_64",
#     exec_compatible_with = [
#         "@platforms//os:windows",
#         "@platforms//cpu:x86_64",
#     ],
#     target_compatible_with = [
#         "@platforms//os:windows",
#         "@platforms//cpu:x86_64",
#     ],
#     toolchain = ":windows_clang_x86_64_cc_toolchain",
#     toolchain_type = "@bazel_tools//tools/cpp:toolchain_type",
# )

############################
# (3) 如果需要 cross compile (Linux -> Windows)
#     再添加一个 cc_toolchain + toolchain(...) 即可
############################
# cc_toolchain(
#     name = "linux_to_windows_cc_toolchain",
#     toolchain_identifier = "linux_to_windows_clang_x86_64",
#     toolchain_config = ":linux_to_windows_clang_x86_64_toolchain_config",
#     ...
# )
# linux_to_windows_clang_x86_64_toolchain_config(
#     name = "linux_to_windows_clang_x86_64_toolchain_config",
# )
# toolchain(
#     name = "linux_to_windows_clang_x86_64",
#     toolchain_type = "@bazel_tools//tools/cpp:toolchain_type",
#     exec_compatible_with = [
#         "@platforms//os:linux",     # 构建机是 linux
#         "@platforms//cpu:x86_64",
#     ],
#     target_compatible_with = [
#         "@platforms//os:windows",  # 目标产物是 windows
#         "@platforms//cpu:x86_64",
#     ],
#     toolchain = ":linux_to_windows_cc_toolchain",
# )
