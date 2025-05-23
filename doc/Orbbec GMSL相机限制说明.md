## 1. 背景说明  
Orbbec 的 Gemini-335Lg、Gemini-345Lg 相机与传统 RGB GMSL 接口相机在数据流、带宽、供电等方面存在较大差异。因此，接入不同 GMSL 解串板时会有不同的使用限制。本文列举了 GMSL 接口相机的相关限制说明。

## 2. 常见场景使用限制

- **GMSL 系列相机**：彩色流支持 YUYV、RGB 格式，不支持 MJPG 格式；RGB 格式由 SDK 内部通过 YUYV 转换获得。
- **G335Lg**：仅支持 Secondary 模式，不支持 Primary 模式，需主机（SoC）下发触发信号。具体方法可参考 SDK 示例（`ob_multi_devices_sync_gmsltrigger.cpp`）。
- **Max96712 解串器 Secondary 模式**：需同步下发 FSIN 信号，否则开关流可能导致深度流停止。
- **同一解串器多相机限制**（如 Max9296、Max96712 LinkA/B、Max96712 LinkC/D）：
    - 一个相机的 RGB 与另一相机的右 IR 不能同时开启
    - 一个相机的 DEPTH 与另一相机的左 IR 不能同时开启
    - 两个相机最多可同时开启 4 路流（需同时满足前两项）
- **1.4.60 之前版本**：
    - Secondary 模式下，需先下发 FSIN 信号，再开流
    - Hardware Trigger 模式需先开流，再下发触发信号