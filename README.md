# MineWorld

## 简介

a simple minecraft clone, written in c++

## 操作

* W/A/S/D : 前进/左/后退/右
* Esc : 按一下暂停，再按继续
* 鼠标左键：破坏方块
* 鼠标右键：放置方块
* T : 敲指令（见**指令**）
* 鼠标滚轮：切换手里拿的方块

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

![image](https://github.com/yuchenxi2000/mineworld/blob/master/screenshots/example1.jpg)

## 编译

1. 下载依赖库（均可在 GitHub 上找到）
   * glew (https://github.com/nigels-com/glew)
   * glfw (https://github.com/glfw/glfw)
   * rapidjson (https://github.com/Tencent/rapidjson)
   * SOIL (https://github.com/kbranigan/Simple-OpenGL-Image-Library)
   * glm (https://github.com/g-truc/glm)
2. 设置 header search path, library search path
3. 在 Windows 下请编译32位程序（选Win32），并与 opengl32.dll 链接。编译完可能需要 glew.dll 才能运行。
4. Mac OS 如果使用的是动态库需与 OpenGL.framework 链接，使用静态库需额外与 Cocoa, CoreVideo, CoreGraphics, CoreFoundation, IOKit 链接。

## 制作自己的材质包

因为我懒，材质包是 minecraft 的材质包。

游戏启动时会寻找 config.json，从里面读入设置。然后在 config.json 里指定的 block file 里读入 block 信息（加载材质包）。

如果想制作材质包，可以参考我写的 block.json。



yuchenxi2000

2019.9.12