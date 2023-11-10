# FLEE

![](https://www.z4a.net/images/2023/11/10/QQ20231110163933.md.png)

## [视频演示](https://www.bilibili.com/video/BV1bm4y1Z717/?spm_id_from=333.999.0.0&vd_source=b7e239e8cad582a1bda55b359c4eef7a)

## 游戏介绍

在游戏中，你的目的是驾驶一辆小车在青青草原上奔腾，通过怪异的传送箱找到火箭后升空逃离该草原。

使用方向键上键控制小车向前移动，WASD进行摄像机视角的调整。

## 开发环境

IDE：Visual Studio Code

编译器：GCC 8.1.0

图形API：OpenGL

开源库：GLFW、GLAD以及ASSIMP

语言：C/C++，GLSL

## 配置

下载VSC后，配置好GCC环境即可运行，其余详情请参考配置文件中的tasks.json。

如果需要重新编译ASSIMP库，请参考：

[记录一次在Windows下使用CMake+MinGW64编译Assimp库的经历](https://zhuanlan.zhihu.com/p/467620741)

编译完成后，将libassimp-X.dll动态库文件和libassimp.dll.a静态库文件以及libzlibstatic.a文件放入配置的位置即可。


