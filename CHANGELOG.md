# Changelog

## v3.1.0

### Build System

- C++ 标准从 C++11 升级到 C++17
- 更新所有子模块到最新版本：
  - GLFW (支持 3.4)
  - GLM (1.0+)
  - RapidJSON
  - SOIL (Simple OpenGL Image Library)
- CMake 构建改进：添加各平台编译指引和 GLEW 安装说明
- MinGW 默认静态链接 libgcc/libstdc++，避免依赖 MSYS2 运行时 DLL
- 添加 `CMAKE_POLICY_VERSION_MINIMUM 3.5` 兼容 SOIL 的旧版 CMakeLists.txt

### macOS 打包

- 新增 `package_macos.sh` 脚本，自动创建 `.app` bundle
- 支持 `cmake --build build --target package` 一键打包
- GLEW dylib 自动打包进 app，使用 `@executable_path` 相对路径加载
- 发布包无需用户安装 Homebrew 即可运行

### 平台兼容性

- GLEW 在 Windows/MinGW 上默认静态链接，macOS 上默认动态链接（Homebrew 只提供动态库）
- 添加 `GLEW_SHARED` CMake 选项允许手动切换

### 文档

- 新增 CLAUDE.md（AI 辅助开发指南）
- 新增 CHANGELOG.md
- 更新 README.md：Windows (MSVC/MSYS2)、macOS、Linux 编译说明
