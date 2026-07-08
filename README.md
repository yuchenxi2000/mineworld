# MineWorld

[English](README-en.md)

## 简介

a simple minecraft clone, written in c++

## 操作

* W/A/S/D : 前进/左/后退/右
* Esc : 按一下暂停，再按继续
* 鼠标左键：破坏方块
* 鼠标右键：放置方块
* T : 敲指令（见**指令**）
* 键盘左右键：切换手里拿的方块

## 指令

* 放置方块

```
set x y z B // 在 (x, y, z) 处放置 ID 为 B 的方块
```

* 查询方块ID

```
query x y z // 返回 (x, y, z) 处的方块 ID
```

* 根据方块名查询ID

```
blockid s // 返回名字为 s 的方块 ID
```

* 查询方块名

```
blockname id // 返回 ID 为 id 的方块名
```

* 列出所有方块

```
blocklist // 返回方块列表
```

* 传送

```
goto x y z // 传送至 (x, y, z)
```

* 说话

```
say words // 在终端打印 words
```

* 调整字体大小

```
font S // 调整字体大小为 S 个 pixel
```

* 清屏

```
clear // 清空屏幕
```

* 退出游戏

```
exit // 退出游戏
```

## 截图

![image](screenshots/example1.jpg)

## 编译

### 前置条件

- CMake 3.12+
- Git（用于克隆仓库和子模块）
- 支持 C++17 的编译器

```bash
git clone --recursive https://github.com/yuchenxi2000/mineworld.git
cd mineworld
```

### Windows

两种方式任选其一。

#### 方式一：MSYS2 UCRT64

1. 安装 [MSYS2](https://www.msys2.org/)，开始菜单搜索msys2，点击运行打开终端

2. 安装编译工具和 GLEW：

   ```bash
   pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-make mingw-w64-ucrt-x86_64-glew
   ```

3. 编译：

   ```bash
   mkdir build && cd build
   cmake .. -G "MinGW Makefiles"
   cmake --build .
   ```

4. 运行（**必须在 `res/` 目录下运行**）：

   ```bash
   cd ../res && ../build/mineworld.exe
   ```

#### 方式二：Visual Studio Build Tools

1. 安装 [Visual Studio Build Tools](https://visualstudio.microsoft.com/zh-hans/downloads/)（在「所有下载」中找「Visual Studio 生成工具」），勾选「使用 C++ 的桌面开发」

2. 下载 [GLEW 预编译包](https://glew.sourceforge.net/)，解压到 `deps/glew-2.3.1/`

3. 打开 **PowerShell**（非 MSYS2 终端），编译 Release 版本：

   ```powershell
   mkdir build_vs; cd build_vs
   cmake .. -G "Visual Studio 17 2022" -DGLEW_ROOT="<path-to>/deps/glew-2.3.1"
   cmake --build . --config Release
   ```

   > `-DGLEW_ROOT` 请替换为解压 GLEW 的实际绝对路径。

   > **注意：** PowerShell 的 PATH 中不能有 MSYS2 的路径（如 `C:\msys64\ucrt64\bin`），否则 MSVC 会错误引用 MinGW 头文件导致编译失败。

4. 运行：

   ```powershell
   cd ../res; ..\build_vs\Release\mineworld.exe
   ```

> 如果 VS 版本不同，调整 `-G` 参数（如 VS 2026 用 `"Visual Studio 18 2026"`）。

### macOS

```bash
brew install glew
mkdir build && cd build
cmake ..
make
cd ../res && ../build/mineworld
```

### Linux

```bash
apt install libglew-dev   # 或对应包管理器
mkdir build && cd build
cmake ..
make
cd ../res && ../build/mineworld
```

### 重要提示

可执行文件**必须在 `res/` 目录下运行**，因为程序需要读取当前目录下的 `config.json`、材质包、着色器等资源文件。

## 制作自己的材质包

因为我懒，材质包是 minecraft 的材质包借过来的。

游戏启动时会寻找 config.json，从里面读入设置。然后在 config.json 里指定的 block file 里读入 block 信息（加载材质包）。

如果想制作材质包，可以参考我写的 block.json。

## 许可

MIT License. 详见 [LICENSE.txt](LICENSE.txt)。

