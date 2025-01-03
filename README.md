# 2024-2025学年计算机图形学大作业
## 项目简介
本项目为2024-2025学年计算机图形学大作业，主要实现了一个基于OpenGL的3D模型渲染器。该渲染器支持加载GLB等常用文件格式的3D模型，并提供了多种渲染模式，包括平移、旋转、缩放等操作。此外，还实现了光照、阴影、纹理映射等功能，以增强模型的视觉效果。

## 面向对象
由于写得比较简单，框架基本沿袭课程参考代码，可以给像我这样的菜鸡指明一点OpenGL的相关内容吧（大佬勿喷QAQ

## 功能介绍
- 利用`Assimp`库，加载`Glb`等文件格式的3D模型
- 支持包括平移、旋转、调整视场角等操作
- 实现了光照、阴影、纹理映射等功能
- 支持用户交互，包括鼠标拖拽、键盘操作等
- 支持多种材质和纹理，使用Phong光照模型，以增强模型的视觉效果
- **详情参阅使用说明书和答题纸**

键鼠操作：
```
==========================================================
Use mouse to control the light position (continously drag).
==========================================================
Keyboard Usage
[Window]
ESC:            Exit
h:              Print help message

[Model]
l/L:            Increase/Decrease exposure
q/Q:            Driving Mode

[Camera]
v/V:            Increase/Decrease the camera field of view
w/W:            Increase/Decrease the camera height
a/A:            Increase/Decrease the camera left/right angle
s/S:            Increase/Decrease the camera forward/backward angle
d/D:            Increase/Decrease the camera up/down angle
space:          Increase the camera height
ctrl:           Decrease the camera height
u/U:            Increase/Decrease the rotate angle
i/I:            Increase/Decrease the up angle
o/O:            Increase/Decrease the camera radius
```


---
### 备注
**因为事情比较多，整体的代码框架延续了课程参考代码的风格，所以代码没有进行太多的优化，逻辑比较混乱，实现的功能也相对简单，请见谅。**

<details>  
<summary>一些感想和背景</summary>

~~肯定不是因为我蔡~~

其实有很多功能想加（前端，速度控制，HDR渲染，人车交互等等），但是时间不够，所以只能先这样了。

实话讲，虽然已经是在ddl前半个月就着手研究了，但是研究assimp耽误了不少时间（ddl前两天才给我整明白，正式开始功能实现，第一次接触建模这些知识），ddl临近这几天非常痛苦，没日没夜地调试，实现功能，觉都没睡多少，~~全靠肾上腺素支撑~~，不过最后还是在大作业轮到我展示之前的两分钟基本上实现了所有功能（核心功能都整上了）。

代码上，很多实现都是充分利用到了C++的特性，指针，面向对象，库等等。有些基础知识还是不太扎实（上学期才刚学了基础的面向对象程序设计），导致在调试上花费了大量的时间……

临近ddl的前三天，前脚刚处理完JAVA的大作业[BigModelChat](https://github.com/RichardLiuda/BigModelChat)，后脚马上接上图形学，几乎是没日没夜地调，学期最后一周的课也没怎么听，都在写这个。

ddl前一天晚上，才下载了Blender这个久负盛名的**开源**软件，花了些时间熟悉了一下功能，就用上了。由于我主要是拿来编辑和导出模型，只是用到了些基础功能，所以对Blender的掌握程度还是很浅的，UV贴图什么的也没能仔细完善，瑕疵很多。

ddl当晚，VS突然出现了bug，应该是头文件和链接库引起的，我用的是preview版本，不知道是不是系统bug，总之把VS更新了，vcpkg重新部署，重新链接所有库，才勉强可以运行。~~当时心态很难评~~

我是提前选修的这门课（大二上），还需要顾及到本学期的其他课程，压力确实比较大，很遗憾没能把所有想要加的功能都实现。但是图形学这方面的内容我是非常感兴趣的，自己平时也玩玩摄影，影像后期什么的，和图形学有很多相似之处。也正是因为感兴趣，ddl这几天才能坚持下来，不然我可能早就开摆了。
</details>
