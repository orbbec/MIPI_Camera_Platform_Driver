# ORBBEC camera driver for GMSL interface

## GSML相机驱动简介
 本仓库主要存放奥比GMSL接口相机的驱动文件，方便开发者/客户更加方便接入GSML接口的相机；
 ### 库目录介绍
- bin：放置奥比GMSL相机适配好的二进制文件，直接集成就可以使用；
- doc：放置GSML接入过程中的文档，以PDF格式为主
- src：kernel目录放置驱动的适配源码；patch目录放置针对patch文件；
- tools：放置验证gmsl正常运行的demo示例
  
### 代码分支说明
  - main：作为正式版本释放的稳定分支，本分支上的代码、二进制文件都是测试通过的版本；
  - gemini-335Lg：适配Gemini-335Lg的开发分支
  - gemini-345lg：适配Gemini-345Lg的开发分支
  
### GMSL接入流程
![](./doc/image/liucheng.png)

## 支持相机和平台
   ### 支持相机
    - Gemini-335Lg
    - Gemini-345Lg
  ### 支持平台
- Jetson AGX Orin DevKit
  - Jetpack 6.0 (r36_release_v3.0)
  - Jetpack 6.1 (r36_release_v4.0)
  - Jetpack 6.2 (r36_release_v4.3)
- MIC-733-AO
  - Jetpack 6.0 (r36_release_v3.0)
  - Jetpack 6.1 (r36_release_v4.0)
  

## GMSL接入操作步骤
  ### 硬件接入要求
  
  
  ### 软件驱动安装
  - 二进制bin驱动安装
    

  - patch驱动安装
  
  - kernel源码安装
  
## GMSL相机性能指标
   gmsl相机接入成功后，需要关注GMSL相机如下指标数据；
   - 时间戳稳定性
   - 帧率稳定性
   - 同步精度
  关于性能指标的测试方法和工具，请参考《性能测试说明.pdf》

## GMSL使用限制说明
  GMSL相机要接入不同的9296/92716等不同的解串芯片；奥比GMSL相机同时支持深度、彩色、IR、IMU等多路流，在使用方面有一些限制；
   - GMSL 只支持V4L2，YUYV格式，不支持MJPG格式，然后RGB是YUYV格式转换得到。
   - G335Lg metadata 是单独的节点，而系列 metadada 是在视频帧里面，这个对客户是透明的
   - 接在同一Max9296、Max96712 LinkA/B 、Max96712 LinkC/D 上的两个相机使用有以下限制
     -  一个相机的RGB 跟另一个相机的右IR 不能同时开流
     -  一个相机的DEPTH 跟另一个相机的左IR 不能同时开
     -  两个相机加起来最多开4路流（同时满足前两个条件即满足这个条件）
    
  针对更多已知的限制条件，参考[GMSL相机使用限制说明](./doc/Orbbec%20GMSL系列相机使用说明.pdf)
## 使用问题反馈

  为了方便众多开发者/客户快速接入gmsl相机，由于各自硬件、软件的差异，可能会遇到各种问题；这些问题可能存在共性，因此采用Github的issue来跟进反馈的问题，针对开发者，规定如下问题反馈流程；
  
- 遇到问题优先在issue中寻找解决方案
- 如果没有检索到，需要新建issue来提交问题
- 提交问题时需要使用issue的模版提交问题描述，可以方便问题快速定位；
  

## 常见问题FAQ
 列举几个常见的问题清单


