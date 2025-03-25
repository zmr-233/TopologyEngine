# BuildMarco.bzl
load("@rules_cc//cc:defs.bzl", "cc_library", "cc_test")


def engine_lib(
    name,
    srcs=[],
    hdrs=[],
    deps=[],
    visibility=None,
    copts=[],
    linkopts=[],
    target_compatible_with=[],
):
    all_copts = [
        "-std=c++23",
        "-O3",
    ] + copts
    cc_library(
        name=name,
        srcs=srcs,
        hdrs=hdrs,
        deps=deps,
        visibility=visibility if visibility else ["//visibility:public"],
        copts=all_copts,
        linkopts=linkopts,
        target_compatible_with=target_compatible_with,
        strip_include_prefix="Public",
        include_prefix="",
    )
    # 移除文件路径中的 "Public/"，使得编译器看到的路径变为 "TypeUtils/...".
    # include_prefix 设为 ""，意思是不再加额外前缀


def engine_test(
    name,
    srcs=[],
    deps=[],
    visibility=None,
    copts=[],
    linkopts=[],
    target_compatible_with=[],
):
    all_copts = [
        "-std=c++23",
        "-O3",
    ] + copts
    cc_test(
        name=name,
        srcs=srcs,
        deps=deps,
        visibility=visibility if visibility else ["//visibility:private"],
        copts=all_copts,
        linkopts=linkopts,
        target_compatible_with=target_compatible_with,
    )


# def editor_lib(...):
#     """若有专门 Editor 模块，可以再封装一层"""
#     # 类似写法，或者定制 Editor 特有编译宏
#     pass

# def plugin_lib(...):
#     """若支持插件化，也可写类似封装"""
#     pass


# def _archive(ctx):  # 定义方法，与 Python 类似，前缀为 _ 的函数一般只有当前文件可见
#     out_file = ctx.actions.declare_file(ctx.attr.out)  # out 是一般属性，保存在 attr 中
#     args = ctx.actions.args()  # args 保存 action 使用的参数，这里的 action 是 zip 命令

#     args.add(out_file)  # zip action 的输出文件名
#     # ctx.files 对象类型类似于 Python 中的 Dict[str, List[File]]
#     args.add_all(ctx.files.files)  # Bazel 对参数有重组行为，细节见下文

#     ctx.actions.run(  # 注册 action 到 Action Graph
#         executable="zip",  # 需要执行的命令
#         arguments=[args],  # action（zip 命令） 使用的参数
#         inputs=ctx.files.files,  # inputs/outputs 是 Bazel 使用的参数，比如判断文件是否存在
#         outputs=[out_file],
#     )
#     # action 可以返回多个结果，所以这里返回的是一个数组
#     return [DefaultInfo(files=depset([out_file]))]


# archive = rule(  # 定义规则
#     implementation=_archive,
#     attrs={
#         # 规则的参数声明需要放在 attrs 字典中，参数有名称和类型，使用内置 attr 对象指定参数类型
#         # 因为 allow_files=True，所以 files 会被保存到 ctx.files 对象中
#         "files": attr.label_list(allow_files=True),
#         "out": attr.string(mandatory=True),
#     },
# )
