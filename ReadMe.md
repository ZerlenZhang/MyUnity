# 仿Unity的OpenGL项目
![Image text](https://raw.githubusercontent.com/hongmaju/light7Local/master/img/productShow/20170518152848.png)

## 已完成功能

### 渲染方面

手动实现一套最最简单的渲染流水线，从VAO，VBO到最终显示到屏幕上三维世界

写了三个常用Shader 天空盒Shader，StandardShader，BoxShader，后两者支持动态添加光照效果（目前最多四种）

对于模型，可以利用其法线贴图，高度贴图，纹理贴图等综合起来显示更为真实的效果，且具有多个可以动态调节的参数

封装Camera功能，成为单独的类，可以同时显示多摄像机同时渲染


### 项目架构

在实现基础渲染功能基础上，模拟Unity的使用习惯，我进行了以下重构：

1、设计Object类，具有name，OnGUI等最基本功能

2、设计出MonoBehavior类，通过C++继承多态实现Start，Undate，OnGUI等函数的系统调用

3、在MonoBehavior基础上，对已有功能模块化封装，抽象出了：Transform，Camera，BoxRender，SkyBoxRender，ModelRender，SpotLIght，PointLight等脚本

4、设计出GameObject类，可以动态添加或获取物体身上挂载的MonoBehavior（当然，用AddComponent<T>和GetComponent<T>），也可以动态添加移出物体，如此一来，给空物体挂载对应脚本就具有相应功能，这与Unity保持了一致

5、设计出Model，Material，Shader等类，与组件系统交互，进一步解耦合

6、封装用户输入成Input类，这样拓展起来就非常方便了，项目里的主摄像机移动就是一个单独的脚本


### UI的实现原理及功能

Ui模块采用ImGUI开源库制作，Object类的OnGUI就是本物体的编辑器显示

UI面板类似Unity中层级面板和Inspector面板的整合，根目录显示场景中所有物体，打开折叠块可以看到物体上挂载的所有脚本，打开脚本折叠块可以看到对应脚本的可调节参数

	Inspector面板Global折叠块下是全局设置，目前只可以修改清屏颜色

	Inspector面板GameObject折叠块会显示场景中存在的所有物体信息，

	打开后，场景中会预制一个平行光和一个camera

	每一个物体都可以点开，下方是它具有的组件信息，点开后可以编辑该物体各个组件的属性

	GameObject菜单用来创建新物体

	Component菜单用来给物体动态添加组件【检视面板中谁打开了给谁添加】

	通过菜单GameObject可以添加物体，光源，模型，天空盒，摄像机等 

	通过菜单Component可以添加光源，摄像机，自转等组件
 

### 键位介绍

	空格——控制鼠标锁定

	锁定时，鼠标隐藏，用于场景漫游，主摄像机会跟随鼠标移动

	解锁时，鼠标出现，主摄像机不在跟随鼠标移动，用于调节面板参数

	WASD——漫游模式控制主摄像机移动，QE控制摄像机Y轴方向移动

## 将来可以添加的功能/模块

### 渲染

更加真是的光照——基于物理的光照渲染（PBR）

粗糙度，金属度不均匀物体的渲染

根据环境实时反射的球面

阴影甚至实时阴影

### 引擎功能方面

更好的资源管理界面，管理模型，Shader等，动态选择和装载

物体的父子级关系

UI面板功能分里，层级面板，检视面板分离

等等


