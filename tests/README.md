# 测试

以下命令从仓库根目录运行。

## C++ 回归与 OpenGL 测试

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_TESTING=ON -DMINEWORLD_GL_TESTS=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

`regression.cpp` 无需窗口；`opengl_smoke.cpp` 使用隐藏窗口，覆盖初始化、渲染、
方块增删、缩放和退出清理，需要桌面图形环境。多配置生成器运行 CTest 时加上 `-C Release`。

## macOS 打包验证

`macos_package_smoke.py` 验证单架构和 universal ZIP 应用包，
只依赖 Python 3 标准库和 macOS 自带工具。它会：

- 将 ZIP 解压到临时目录，校验 Info.plist、完整应用签名和指定架构。
- 检查动态 GLEW 随包分发及实际加载路径；也支持静态 GLEW。
- 从临时工作目录强制以指定架构运行应用，验证包内资源查找和纹理初始化。
- 每次启动运行 5 秒后终止进程并自动删除临时目录。
- 指定 `--test-build` 时，另外运行对应架构的 C++ 回归和 OpenGL 测试。

测试需要桌面图形环境，会短暂显示游戏窗口并捕获鼠标。Apple Silicon 上运行
`x86_64` 需要已安装 Rosetta；不支持的架构会报错，不会跳过。5 秒启动检查不验证
游戏正常退出流程；正常退出由 OpenGL 冒烟测试覆盖。

### Apple Silicon 包

```bash
cmake --build build --target package
ditto -c -k --keepParent build/MineWorld.app build/mineworld-macos.zip
python3 tests/macos_package_smoke.py build/mineworld-macos.zip --test-build build
```

省略 `--test-build` 可以只验证 ZIP 应用包，默认运行本机架构。

### Universal 包

```bash
bash package_universal.sh
ditto -c -k --keepParent build_universal/MineWorld.app \
  build_universal/mineworld-macos-universal.zip

cmake -S . -B build_universal_tests \
  '-DCMAKE_OSX_ARCHITECTURES=arm64;x86_64' \
  -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15 \
  -DGLEW_CUSTOM_PATH="$PWD/deps/glew-universal" \
  -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=ON -DMINEWORLD_GL_TESTS=ON
cmake --build build_universal_tests --parallel

python3 tests/macos_package_smoke.py \
  build_universal/mineworld-macos-universal.zip \
  --architectures arm64 x86_64 --test-build build_universal_tests
```

`package_universal.sh` 默认将 Intel 发布目标设为 macOS 10.13，Apple Silicon 为 11.0。
回归测试使用的 `std::filesystem` 要求 macOS 10.15，因此测试使用单独的构建目录，
不改变发布包的最低目标版本。保持 `build_universal` 的 `BUILD_TESTING=OFF`（默认值）。
