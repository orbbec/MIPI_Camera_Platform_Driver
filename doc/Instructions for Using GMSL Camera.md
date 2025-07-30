## Background notes
Orbbec Gemini-335Lg, Gemini345Lg camera; Different from the traditional RGB gmsl interface camera, the data flow, data
bandwidth and power supply requirements are very different. Therefore, when accessing different gmsl deserializer boards, there
will be different restricted usage scenarios;This article lists the gmsl interface of the use of the camera restrictions.
## Restrictions for Common Scenarios
  - GMSL series cameras, color streams support YUYV and RGB formats, but do not support MJPG formats; RGB formats are
converted from YUYV formats within the SDK.
- G335Lg can only support Secondary mode but not Primary mode. The host (soc) must send a trigger signal. Refer to SDK
Sample.cpp)
- When using Max96712 deserializer in Secondary mode, it is necessary to send fsin signal at the same time. Otherwise, the
switch flow will cause deep stop
- Two cameras connected to the same Max9296, Max96712 LinkA/B, Max96712 LinkC/D have the following restrictions:
    - Before driver version v12.02, there was a restriction that the RGB of one camera and the right IR of another camera could not stream simultaneously. After driver version v12.02, the restriction was modified to that the RGB of one camera and the left IR of another camera cannot stream simultaneously.
    - Before driver version v12.02, there was a restriction that the DEPTH of one camera and the left IR of another camera could not stream simultaneously. After driver version v12.02, the restriction was modified to that the DEPTH of one camera and the right IR of another camera cannot stream simultaneously
    - Two cameras add up to 4 flows (This condition is met when the first two conditions are met at the same time)
- No.In the Secondary mode of the previous version of 4.60, the fsin signal is sent first, and then the flow is opened. In the
Hardware Trigger mode, the flow is opened first and then the Trigger signal is sent.
