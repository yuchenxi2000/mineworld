# MineWorld

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

![image](https://github.com/yuchenxi2000/mineworld/blob/master/screenshots/example1.jpg)

## 编译

GitHub Release里的支持macOS 10.13及以上、Windows 8.1及以上，其他系统可以自行编译。

为方便编译已经改为CMake编译系统，本人在macOS 11.5.2下测试成功，如果在其他机器上失败欢迎提issue或者pull request。glew不方便放进CMake，请安装二进制库或者自己编译。

1. clone本仓库

2. 安装glew（或者自行编译）

   从 http://glew.sourceforge.net/ （或者 https://github.com/nigels-com/glew ）下载安装，macOS下可以用brew

3. cmake编译即可

注意可执行文件必须放在res目录下，也就是说启动目录下必须有config.json等文件，因为程序运行时需要读取配置文件/材质包等资源文件。

## 制作自己的材质包

因为我懒，材质包是 minecraft 的材质包借过来的。

游戏启动时会寻找 config.json，从里面读入设置。然后在 config.json 里指定的 block file 里读入 block 信息（加载材质包）。

如果想制作材质包，可以参考我写的 block.json。



yuchenxi2000

2021.9.12

