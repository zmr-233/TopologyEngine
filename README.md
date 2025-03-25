# Topology Engine

Topology Engine 是为 **Topology Atomicverse | 拓扑幻境** 的体素游戏而开发的游戏引擎。该引擎不仅为体素世界的构建提供支持，还作为一系列技术试验的场地，包括 Unreal Engine 相关技术、Bazel 构建系统等。

## 目录结构

```
├── Engine                     # 游戏引擎核心部分
│   ├── Config                 # 配置文件
│   ├── Documentation          # 文档
│   ├── Editor                 # 编辑器
│   ├── MODULE.bazel           # Bazel 构建文件
│   ├── Runtime                # 引擎运行时
│   │   ├── Core               # 核心功能
│   │   └── Render             # 渲染模块
│   ├── Shader                 # 着色器相关文件
│   ├── Source                 # 引擎源码
│   └── Tools                  # 工具类文件
├── Projects                   # 项目文件
│   └── 01_LearnGL             # LearnOpenGL 项目
├── README.md                  # 项目说明文件
└── Thirdparty                 # 第三方依赖库
```

## 目标

Topology Engine 的设计目标是为 **拓扑幻境** 的体素游戏提供一个高效且可扩展的引擎架构，支持体素世界的构建与渲染。与此同时，它也作为一个技术试验平台，用于验证和实验现代游戏引擎技术，包括但不限于：

- **体素渲染技术**：支持大规模体素世界的构建与动态渲染。
- **Unreal Engine 技术集成**：探索与 Unreal Engine 的融合与技术实现。
- **Bazel 构建系统**：使用 Bazel 构建工具来提高构建效率与可维护性。

## 安装与使用

### 系统要求
- 操作系统：Linux / Windows（具体实现取决于模块）
- 依赖：Bazel 构建工具

### 构建与运行
1. 克隆本仓库：
    ```
    git clone <repository_url>
    ```

2. 安装 Bazel（若尚未安装）：
    - [Bazel 官方安装指南](https://bazel.build)

3. 在项目根目录直接启动构建：
    ```
    bazel test @engine//Runtime/Core:TypeUtilsTest
    bazel run @learngl//:learngl_app
    ```


## 项目结构说明

- **Engine**: 包含引擎的核心逻辑、运行时环境、着色器和工具等模块。
- **Projects**: 包含与引擎相关的具体项目，如 OpenGL 学习项目（`01_LearnGL`）。
- **Thirdparty**: 存放第三方依赖库。
- **Documentation**: 提供详细的技术文档，帮助开发者理解引擎架构与使用方法。

## 贡献

欢迎提交问题和贡献代码！如果您有任何建议或发现bug，请通过 issues 或 pull requests 与我们联系。

## 许可证

本项目遵循 [MIT 许可证](LICENSE)。

---

zmr_233 (zmr_233@outlook.com)