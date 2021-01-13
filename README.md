# ToyMC
This is the Final Project of Computer Graphics 100433 course.

本项目基于Modern OpenGL和C++实现了一个Minecraft沙盒游戏的Toy Demo。项目还原了Minecraft中部分种类的方块和场景，卡通效果的光照渲染，运动逻辑以及挖掘和放置方块。由于Minecraft的地图随机生成较为复杂，因此我们参考网上的一些地图生成数学模型，实现了较为简单几种场景的随机地图的无限生成。

Demo

Daytime

![image-20210112122238475](https://github.com/SAOHPRWHG/ToyMC/blob/main/Fig/Daytime.png)

Night

![image-20210112122238475](https://github.com/SAOHPRWHG/ToyMC/blob/main/Fig/Night.png)



#### 安装与编译

本项目使用的OpenGL管理库为glfw和glad，在Library文件夹中已全部包含。

请安装跨平台编译工具cmake

并使用以下步骤编译

```
mkdir Build
cd Build
```

若采用Mingw编译器请使用

```
cmake .. -G "MinGW Makefiles"
```

若使用Visual Studio系列编译器，请使用cmake-gui进行编译，编译平台选项选择“Win32”

![image-20210112122238475](https://github.com/SAOHPRWHG/ToyMC/blob/main/Fig/cmake-gui.png)

如想直接下载可执行文件可从本页面右侧Releases中下载。

本项目的地图生成策略参考了https://github.com/Hopson97/MineCraft-One-Week-Challenge 中的数学模型，数据库和选择性渲染等部分实现细节参考了 https://www.michaelfogleman.com/projects/craft/

