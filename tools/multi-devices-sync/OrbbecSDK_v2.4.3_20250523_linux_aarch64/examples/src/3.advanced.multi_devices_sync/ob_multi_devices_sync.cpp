// Copyright (c) Orbbec Inc. All Rights Reserved.
// Licensed under the MIT License.

#include <libobsensor/ObSensor.hpp>

#include "utils.hpp"
#include "utils_opencv.hpp"
#include "utils/cJSON.h"

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <fstream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <chrono>

#define MAX_DEVICE_COUNT 9
#define CONFIG_FILE "./MultiDeviceSyncConfig.json"
#define KEY_ESC 27

static bool quitStreamPreview = false;

OBFormat textToOBFormat(const char *text) {
    if(strcmp(text, "YUYV") == 0) {
        return OB_FORMAT_YUYV;
    }
    else if(strcmp(text, "YUY2") == 0) {
        return OB_FORMAT_YUY2;
    }
    else if(strcmp(text, "UYUV") == 0) {
        return OB_FORMAT_UYVY;
    }
    else if(strcmp(text, "NV12") == 0) {
        return OB_FORMAT_NV12;
    }
    else if(strcmp(text, "NV21") == 0) {
        return OB_FORMAT_NV21;
    }
    else if(strcmp(text, "MJPEG") == 0) {
        return OB_FORMAT_MJPEG;
    }
    else if(strcmp(text, "MJPG") == 0) {
        return OB_FORMAT_MJPG;
    }
    else if(strcmp(text, "H264") == 0) {
        return OB_FORMAT_H264;
    }
    else if(strcmp(text, "H265") == 0) {
        return OB_FORMAT_H265;
    }
    else if(strcmp(text, "Y8") == 0) {
        return OB_FORMAT_Y8;
    }
    else if(strcmp(text, "Y10") == 0) {
        return OB_FORMAT_Y10;
    }
    else if(strcmp(text, "Y12") == 0) {
        return OB_FORMAT_Y12;
    }
    else if(strcmp(text, "Y14") == 0) {
        return OB_FORMAT_Y14;
    }
    else if(strcmp(text, "Y16") == 0) {
        return OB_FORMAT_Y16;
    }
    else if(strcmp(text, "GRAY") == 0) {
        return OB_FORMAT_GRAY;
    }
    else if(strcmp(text, "RLE") == 0) {
        return OB_FORMAT_RLE;
    }
    else if(strcmp(text, "RGB") == 0) {
        return OB_FORMAT_RGB;
    }
    else if(strcmp(text, "RGB888") == 0) {
        return OB_FORMAT_RGB888;
    }
    else if(strcmp(text, "BGR") == 0) {
        return OB_FORMAT_BGR;
    }
    else if(strcmp(text, "BGRA") == 0) {
        return OB_FORMAT_BGRA;
    }
    else if(strcmp(text, "COMPRESSED") == 0) {
        return OB_FORMAT_COMPRESSED;
    }
    else if(strcmp(text, "RVL") == 0) {
        return OB_FORMAT_RVL;
    }
    return OB_FORMAT_UNKNOWN;
}

typedef struct DeviceConfigInfo_t {
    std::string             deviceSN;
    OBMultiDeviceSyncConfig syncConfig;
} DeviceConfigInfo;

typedef struct StreamConfigInfo_t {
    OBSensorType sensorType;
    int          width;
    int          height;
    OBFormat     format;
    int          fps;
    bool         enable;
} StreamConfigInfo;

typedef struct SensorControlInfo_t {
    bool        enable;
    std::string depthPreset;           // Default
    int         laserPowerLevel;       // 0~6
    int         disparitySearchRange;  // 0:64,1:128,2:256
    bool        hardwareD2d;           // true: hw d2d, false: sw d2d
    bool        hardwareNoiseRemoval;  // true: hardware noise removal filter enable, false: hardware noise removal filter enable
    bool        softwareNoiseRemoval;  // true: software noise removal filter enable, false: software noise removal filter enable
    bool        laserOn;
    bool        colorAutoExposure;
    int         colorExposure;
    int         colorGain;
    bool        irAutoExposure;
    int         irExposure;
    int         irGain;
} SensorControlInfo;

typedef struct PipelineHolder_t {
    std::shared_ptr<ob::Pipeline> pipeline;
    OBSensorType                  sensorType;
    int                           deviceIndex;
    std::string                   deviceSN;
} PipelineHolder;

std::ostream &operator<<(std::ostream &os, const PipelineHolder &holder);
std::ostream &operator<<(std::ostream &os, std::shared_ptr<PipelineHolder> holder) {
    return os << *holder;
}

std::mutex                                    frameMutex;
std::map<uint8_t, std::shared_ptr<ob::Frame>> colorFrames;
std::map<uint8_t, std::shared_ptr<ob::Frame>> depthFrames;

std::vector<std::shared_ptr<ob::Device>>       streamDevList;
std::vector<std::shared_ptr<ob::Device>>       configDevList;
std::vector<std::shared_ptr<DeviceConfigInfo>> deviceConfigList;
std::vector<std::shared_ptr<StreamConfigInfo>> streamConfigList;
std::shared_ptr<SensorControlInfo>             sensorControlInfo = nullptr;

std::condition_variable                      waitRebootCompleteCondition;
std::mutex                                   rebootingDevInfoListMutex;
std::vector<std::shared_ptr<ob::DeviceInfo>> rebootingDevInfoList;
std::vector<std::shared_ptr<PipelineHolder>> pipelineHolderList;

bool loadConfigFile();
int  configMultiDeviceSync();
int  testMultiDeviceSync();

void startStream(std::shared_ptr<PipelineHolder> pipelineHolder);
void stopStream(std::shared_ptr<PipelineHolder> pipelineHolder);

void handleColorStream(uint8_t devIndex, std::shared_ptr<ob::Frame> frame);
void handleDepthStream(uint8_t devIndex, std::shared_ptr<ob::Frame> frame);

std::string           OBSyncModeToString(const OBMultiDeviceSyncMode syncMode);
OBMultiDeviceSyncMode stringToOBSyncMode(const std::string &modeString);

OBFrameType mapFrameType(OBSensorType sensorType);
std::string readFileContent(const char *filePath);

int  strcmp_nocase(const char *str0, const char *str1);
bool checkDevicesWithDeviceConfigs(const std::vector<std::shared_ptr<ob::Device>> &deviceList);

std::shared_ptr<PipelineHolder> createPipelineHolder(std::shared_ptr<ob::Device> device, OBSensorType sensorType, int deviceIndex);

ob::Context context;

int main(void) try {
    int                       choice;
    int                       exitValue      = 0;
    constexpr std::streamsize maxInputIgnore = 10000;

    while(true) {
        std::cout << "\n--------------------------------------------------\n";
        std::cout << "Please select options: \n";
        std::cout << " 0 --> config devices sync mode. \n";
        std::cout << " 1 --> start stream \n";
        std::cout << "--------------------------------------------------\n";
        std::cout << "Please select input: ";
        // std::cin >> choice;
        if(!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(maxInputIgnore, '\n');
            std::cout << "Invalid input. Please enter a number [0~1]" << std::endl;
            continue;
        }
        std::cout << std::endl;

        if(!loadConfigFile()) {
            std::cout << "load config failed" << std::endl;
            return -1;
        }

        switch(choice) {
        case 0:
            exitValue = configMultiDeviceSync();
            if(exitValue == 0) {
                std::cout << "Config MultiDeviceSync Success. \n" << std::endl;

                exitValue = testMultiDeviceSync();
            }
            break;
        case 1:
            std::cout << "\nStart Devices video stream." << std::endl;
            exitValue = testMultiDeviceSync();
            break;
        }

        if(exitValue == 0) {
            break;
        }
    }
    return exitValue;
}
catch(ob::Error &e) {
    std::cerr << "function:" << e.getFunction() << "\nargs:" << e.getArgs() << "\nmessage:" << e.what() << "\ntype:" << e.getExceptionType() << std::endl;
    std::cout << "\nPress any key to exit.";
    ob_smpl::waitForKeyPressed();
    exit(EXIT_FAILURE);
}

bool isValidStreamConfigInfo(const StreamConfigInfo &configInfo) {
    return configInfo.width > 0 && configInfo.height > 0 && configInfo.fps > 0 && configInfo.format != OB_FORMAT_UNKNOWN;
}
bool isValidStreamConfigInfo(std::shared_ptr<StreamConfigInfo> configInfo) {
    return configInfo && isValidStreamConfigInfo(*configInfo);
}
int configMultiDeviceSync() {
    try {

        if(deviceConfigList.empty()) {
            std::cout << "DeviceConfigList is empty. please check config file: " << CONFIG_FILE << std::endl;
            return -1;
        }

        if(streamConfigList.empty()) {
            std::cout << "streamConfigList is empty. please check config file (streamProfile): " << CONFIG_FILE << std::endl;
            return -1;
        }

        if(sensorControlInfo == nullptr) {
            std::cout << "streamConfigList is empty. please check config file (sensorControl): " << CONFIG_FILE << std::endl;
            return -1;
        }

        // Query the list of connected devices
        auto devList  = context.queryDeviceList();
        int  devCount = devList->deviceCount();
        for(int i = 0; i < devCount; i++) {
            std::shared_ptr<ob::Device> device = devList->getDevice(i);
            configDevList.push_back(devList->getDevice(i));
        }

        if(configDevList.empty()) {
            std::cerr << "Device list is empty. please check device connection state" << std::endl;
            return -1;
        }

        // write configuration to device
        for(auto config: deviceConfigList) {
            auto findItr = std::find_if(configDevList.begin(), configDevList.end(), [config](std::shared_ptr<ob::Device> device) {
                auto serialNumber = device->getDeviceInfo()->serialNumber();
                return strcmp_nocase(serialNumber, config->deviceSN.c_str()) == 0;
            });
            if(findItr != configDevList.end()) {
                auto device    = (*findItr);
                auto curConfig = device->getMultiDeviceSyncConfig();
                // Update the configuration items of the configuration file, and keep the original configuration for other items
                curConfig.syncMode             = config->syncConfig.syncMode;
                curConfig.depthDelayUs         = config->syncConfig.depthDelayUs;
                curConfig.colorDelayUs         = config->syncConfig.colorDelayUs;
                curConfig.trigger2ImageDelayUs = config->syncConfig.trigger2ImageDelayUs;
                curConfig.triggerOutEnable     = config->syncConfig.triggerOutEnable;
                curConfig.triggerOutDelayUs    = config->syncConfig.triggerOutDelayUs;
                curConfig.framesPerTrigger     = config->syncConfig.framesPerTrigger;
                std::cout << "-Config Device syncMode:" << curConfig.syncMode << ", syncModeStr:" << OBSyncModeToString(curConfig.syncMode) << std::endl;
                device->setMultiDeviceSyncConfig(curConfig);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        return 0;
    }
    catch(ob::Error &e) {
        std::cerr << "configMultiDeviceSync failed! \n";
        std::cerr << "function:" << e.getName() << "\nargs:" << e.getArgs() << "\nmessage:" << e.getMessage() << "\ntype:" << e.getExceptionType() << std::endl;
        return -1;
    }
}

void startDeviceStreams(const std::vector<std::shared_ptr<ob::Device>> &devices, int startIndex) {
    std::vector<OBSensorType> sensorTypes = { OB_SENSOR_DEPTH, OB_SENSOR_COLOR };
    for(auto &dev: devices) {
        if(sensorControlInfo->enable) {
            // set param
            try {
                dev->loadPreset(sensorControlInfo->depthPreset.c_str());
                dev->setIntProperty(OB_PROP_LASER_POWER_LEVEL_CONTROL_INT, sensorControlInfo->laserPowerLevel);
                dev->setIntProperty(OB_PROP_DISP_SEARCH_RANGE_MODE_INT, sensorControlInfo->disparitySearchRange);
                // true: hardware d2d, false: software d2d
                dev->setBoolProperty(OB_PROP_DISPARITY_TO_DEPTH_BOOL, sensorControlInfo->hardwareD2d);
                dev->setBoolProperty(OB_PROP_LASER_CONTROL_INT, sensorControlInfo->laserOn);
                dev->setBoolProperty(OB_PROP_COLOR_AUTO_EXPOSURE_BOOL, sensorControlInfo->colorAutoExposure);
                if(!sensorControlInfo->colorAutoExposure) {
                    dev->setIntProperty(OB_PROP_COLOR_EXPOSURE_INT, sensorControlInfo->colorExposure);
                    dev->setIntProperty(OB_PROP_COLOR_GAIN_INT, sensorControlInfo->colorGain);
                }
                dev->setBoolProperty(OB_PROP_IR_AUTO_EXPOSURE_BOOL, sensorControlInfo->irAutoExposure);
                if(!sensorControlInfo->irAutoExposure) {
                    dev->setIntProperty(OB_PROP_IR_EXPOSURE_INT, sensorControlInfo->irExposure);
                    dev->setIntProperty(OB_PROP_IR_GAIN_INT, sensorControlInfo->irGain);
                }

                dev->setBoolProperty(OB_PROP_HW_NOISE_REMOVE_FILTER_ENABLE_BOOL, sensorControlInfo->hardwareNoiseRemoval);
                dev->setBoolProperty(OB_PROP_DEPTH_NOISE_REMOVAL_FILTER_BOOL, sensorControlInfo->softwareNoiseRemoval);
            }
            catch(ob::Error &e) {
                std::cerr << "set camera param failed! \n";
                std::cerr << "function:" << e.getName() << "\nargs:" << e.getArgs() << "\nmessage:" << e.getMessage() << "\ntype:" << e.getExceptionType()
                          << std::endl;
            }
        }

        for(auto sensorType: sensorTypes) {
            auto holder = createPipelineHolder(dev, sensorType, startIndex);
            pipelineHolderList.push_back(holder);
            startStream(holder);
        }

        startIndex++;
    }
    quitStreamPreview = false;
}

// key press event processing
void handleKeyPress(ob_smpl::CVWindow &win, int key) {
    // Get the key value
    if(key == KEY_ESC) {
        if(!quitStreamPreview) {
            win.setShowInfo(false);
            win.setShowSyncTimeInfo(false);
            quitStreamPreview = true;
            win.close();
            win.destroyWindow();
            std::cout << "press ESC quitStreamPreview" << std::endl;
        }
    }
    else if(key == 'S' || key == 's') {
        std::cout << "syncDevicesTime..." << std::endl;
        context.enableDeviceClockSync(60000);  // Manual update synchronization
    }
    else if(key == 'T' || key == 't') {
        // software trigger
        std::cout << "check software trigger mode" << std::endl;
        for(auto &dev: streamDevList) {
            auto multiDeviceSyncConfig = dev->getMultiDeviceSyncConfig();
            if(multiDeviceSyncConfig.syncMode == OB_MULTI_DEVICE_SYNC_MODE_SOFTWARE_TRIGGERING) {
                std::cout << "software trigger..." << std::endl;
                dev->triggerCapture();
            }
        }
    }
}

int testMultiDeviceSync() {
    try {
        streamDevList.clear();
        // Query the list of connected devices
        auto devList  = context.queryDeviceList();
        int  devCount = devList->deviceCount();
        for(int i = 0; i < devCount; i++) {
            streamDevList.push_back(devList->getDevice(i));
        }

        if(streamDevList.empty()) {
            std::cerr << "Device list is empty. please check device connection state" << std::endl;
            return -1;
        }

        // traverse the device list and create the device
        std::vector<std::shared_ptr<ob::Device>> primary_devices;
        std::vector<std::shared_ptr<ob::Device>> secondary_devices;
        for(auto dev: streamDevList) {
            auto config = dev->getMultiDeviceSyncConfig();
            if(config.syncMode == OB_MULTI_DEVICE_SYNC_MODE_PRIMARY) {
                primary_devices.push_back(dev);
            }
            else {
                secondary_devices.push_back(dev);
            }
        }

        std::cout << "Secondary devices start..." << std::endl;
        startDeviceStreams(secondary_devices, 0);

        if(primary_devices.empty()) {
            std::cerr << "WARNING primary_devices is empty!!!" << std::endl;
        }
        else {
            std::cout << "Primary device start..." << std::endl;
            startDeviceStreams(primary_devices, static_cast<int>(secondary_devices.size()));
        }

        // Start the multi-device time synchronization function
        context.enableDeviceClockSync(60000);  // update and sync every minitor

        // Create a window for rendering and set the resolution of the window
        ob_smpl::CVWindow win("MultiDeviceSyncViewer", 1600, 900, ob_smpl::ARRANGE_GRID);

        // set key prompt
        win.setKeyPrompt("'S': syncDevicesTime, 'T': software trigger");
        // set the callback function for the window to handle key press events
        win.setKeyPressedCallback([&](int key) { handleKeyPress(win, key); });

        win.setShowInfo(true);
        win.setShowSyncTimeInfo(true);
        while(win.run() && !quitStreamPreview) {
            if(quitStreamPreview) {
                break;
            }
            std::vector<std::pair<std::shared_ptr<ob::Frame>, std::shared_ptr<ob::Frame>>> framePairs;
            {
                std::lock_guard<std::mutex> lock(frameMutex);
                for(uint8_t i = 0; i < static_cast<uint8_t>(std::min(MAX_DEVICE_COUNT, (int)depthFrames.size())); i++) {
                    if(depthFrames[i] != nullptr && colorFrames[i] != nullptr) {
                        framePairs.emplace_back(depthFrames[i], colorFrames[i]);
                    }
                }
            }
            auto groudID = 0;
            for(const auto &pair: framePairs) {
                groudID++;
                win.pushFramesToView({ pair.first, pair.second }, groudID);
            }
        }

        // Stop streams and clear resources
        for(auto &holder: pipelineHolderList) {
            stopStream(holder);
        }
        pipelineHolderList.clear();
        depthFrames.clear();
        colorFrames.clear();

        // Release resource
        streamDevList.clear();
        configDevList.clear();
        deviceConfigList.clear();
        return 0;
    }
    catch(ob::Error &e) {
        std::cerr << "function:" << e.getName() << "\nargs:" << e.getArgs() << "\nmessage:" << e.getMessage() << "\ntype:" << e.getExceptionType() << std::endl;
        std::cout << "\nPress any key to exit.";
        ob_smpl::waitForKeyPressed();
        exit(EXIT_FAILURE);
        return -1;
    }
}

std::shared_ptr<PipelineHolder> createPipelineHolder(std::shared_ptr<ob::Device> device, OBSensorType sensorType, int deviceIndex) {
    auto holder         = std::make_shared<PipelineHolder>();
    holder->pipeline    = std::make_shared<ob::Pipeline>(device);
    holder->sensorType  = sensorType;
    holder->deviceIndex = deviceIndex;
    holder->deviceSN    = device->getDeviceInfo()->serialNumber();
    return holder;
}

void processFrame(std::shared_ptr<ob::FrameSet> frameSet, OBFrameType frameType, int deviceIndex) {
    if(!frameSet) {
        std::cerr << "Invalid frameSet received." << std::endl;
        return;
    }

    if(quitStreamPreview) {
        // std::cerr << "press ESC quit Stream ProcessFrame." << std::endl;
        return;
    }

    auto frame = frameSet->getFrame(frameType);
    if(frame) {
        if(frameType == OB_FRAME_COLOR) {
            handleColorStream(static_cast<uint8_t>(deviceIndex), frame);
        }
        else if(frameType == OB_FRAME_DEPTH) {
            handleDepthStream(static_cast<uint8_t>(deviceIndex), frame);
        }
    }
}

void handleStreamError(const ob::Error &e) {
    // Separate error handling logic
    std::cerr << "Function: " << e.getName() << "\nArgs: " << e.getArgs() << "\nMessage: " << e.getMessage() << "\nType: " << e.getExceptionType() << std::endl;
}

void startStream(std::shared_ptr<PipelineHolder> holder) {
    std::cout << "startStream. " << holder << std::endl;
    try {
        auto pipeline    = holder->pipeline;
        auto profileList = pipeline->getStreamProfileList(holder->sensorType);

        int      width  = OB_WIDTH_ANY;
        int      height = OB_HEIGHT_ANY;
        OBFormat format = OB_FORMAT_UNKNOWN;
        int      fps    = OB_FPS_ANY;

        for(auto config: streamConfigList) {
            if(config->sensorType == holder->sensorType && isValidStreamConfigInfo(config)) {
                width  = config->width;
                height = config->height;
                format = config->format;
                fps    = config->fps;
                break;
            }
        }

        auto streamProfile = profileList->getVideoStreamProfile(width, height, format, fps);

        auto frameType   = mapFrameType(holder->sensorType);
        auto deviceIndex = holder->deviceIndex;

        std::shared_ptr<ob::Config> config = std::make_shared<ob::Config>();
        config->enableStream(streamProfile);

        pipeline->start(config, [frameType, deviceIndex](std::shared_ptr<ob::FrameSet> frameSet) { processFrame(frameSet, frameType, deviceIndex); });
    }
    catch(ob::Error &e) {
        std::cerr << "starting stream failed: " << holder << std::endl;
        handleStreamError(e);
    }
}

void stopStream(std::shared_ptr<PipelineHolder> holder) {
    try {
        std::cout << "stopStream " << holder << std::endl;
        holder->pipeline->stop();
    }
    catch(ob::Error &e) {
        std::cerr << "stopping stream failed: " << holder << std::endl;
        std::cerr << "function:" << e.getName() << "\nargs:" << e.getArgs() << "\nmessage:" << e.getMessage() << "\ntype:" << e.getExceptionType() << std::endl;
    }
}

void handleStream(uint8_t devIndex, std::shared_ptr<ob::Frame> frame, const char *frameType) {
    std::lock_guard<std::mutex> lock(frameMutex);
    std::cout << "Device#" << static_cast<int>(devIndex) << ", " << frameType << " frame(us) "
              << ", frame timestamp=" << frame->timeStampUs() << ","
              << "global timestamp = " << frame->globalTimeStampUs() << ","
              << "system timestamp = " << frame->systemTimeStampUs() << std::endl;

    if(strcmp(frameType, "color") == 0) {
        colorFrames[devIndex] = frame;
    }
    else if(strcmp(frameType, "depth") == 0) {
        depthFrames[devIndex] = frame;
    }
}

void handleColorStream(uint8_t devIndex, std::shared_ptr<ob::Frame> frame) {
    handleStream(devIndex, frame, "color");
}

void handleDepthStream(uint8_t devIndex, std::shared_ptr<ob::Frame> frame) {
    handleStream(devIndex, frame, "depth");
}

std::string readFileContent(const char *filePath) {
    std::ostringstream oss;
    std::ifstream      file(filePath, std::fstream::in);
    if(!file.is_open()) {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return "";
    }
    oss << file.rdbuf();
    file.close();
    return oss.str();
}

std::shared_ptr<StreamConfigInfo> parseStreamProfileConfig(cJSON *streamProfileElem, const std::string &sensorKey) {
    cJSON *sensorElem = cJSON_GetObjectItem(streamProfileElem, sensorKey.c_str());
    if(!sensorElem) {
        std::cout << "parseConfig[" << sensorKey << "] not find root json element" << std::endl;
        return nullptr;
    }

    auto   streamConfigInfo = std::shared_ptr<StreamConfigInfo>(new StreamConfigInfo{});
    cJSON *widthElem        = cJSON_GetObjectItemCaseSensitive(sensorElem, "width");
    if(widthElem && cJSON_IsNumber(widthElem)) {
        streamConfigInfo->width = widthElem->valueint;
    }
    else {
        std::cout << "parseConfig[" << sensorKey << "] not find 'width' json element" << std::endl;
    }

    cJSON *heightElem = cJSON_GetObjectItemCaseSensitive(sensorElem, "height");
    if(heightElem && cJSON_IsNumber(heightElem)) {
        streamConfigInfo->height = heightElem->valueint;
    }
    else {
        std::cout << "parseConfig[" << sensorKey << "] not find 'height' json element" << std::endl;
    }

    cJSON *formatElem = cJSON_GetObjectItemCaseSensitive(sensorElem, "format");
    if(formatElem && cJSON_IsString(formatElem)) {
        streamConfigInfo->format = textToOBFormat(formatElem->valuestring);
    }
    else {
        std::cout << "parseConfig[" << sensorKey << "] not find 'format' json element" << std::endl;
    }

    cJSON *fpsElem = cJSON_GetObjectItemCaseSensitive(sensorElem, "fps");
    if(fpsElem && cJSON_IsNumber(fpsElem)) {
        streamConfigInfo->fps = fpsElem->valueint;
    }
    else {
        std::cout << "parseConfig[" << sensorKey << "] not find 'fps' json element" << std::endl;
    }

    cJSON *enableElem = cJSON_GetObjectItemCaseSensitive(sensorElem, "enable");
    if(enableElem && cJSON_IsBool(enableElem)) {
        streamConfigInfo->enable = (bool)enableElem->valueint;
    }
    if(!isValidStreamConfigInfo(streamConfigInfo)) {
        std::cout << "parseConfig[" << sensorKey << "] StreamConfigInfo is invalid." << std::endl;
        return nullptr;
    }
    return streamConfigInfo;
}

bool loadConfigFile() {
    int                               deviceCount   = 0;
    std::shared_ptr<DeviceConfigInfo> devConfigInfo = nullptr;
    cJSON                            *deviceElem    = nullptr;

    auto content = readFileContent(CONFIG_FILE);
    if(content.empty()) {
        std::cerr << "load config file failed." << std::endl;
        return false;
    }

    cJSON *rootElem = cJSON_Parse(content.c_str());
    if(rootElem == nullptr) {
        const char *errMsg = cJSON_GetErrorPtr();
        std::cout << std::string(errMsg) << std::endl;
        cJSON_Delete(rootElem);
        return true;
    }

    // 1. multi device sync config
    cJSON *devicesElem = cJSON_GetObjectItem(rootElem, "devices");
    cJSON_ArrayForEach(deviceElem, devicesElem) {
        devConfigInfo = std::make_shared<DeviceConfigInfo>();
        memset(&devConfigInfo->syncConfig, 0, sizeof(devConfigInfo->syncConfig));
        devConfigInfo->syncConfig.syncMode = OB_MULTI_DEVICE_SYNC_MODE_FREE_RUN;

        cJSON *snElem = cJSON_GetObjectItem(deviceElem, "sn");
        if(cJSON_IsString(snElem) && snElem->valuestring != nullptr) {
            devConfigInfo->deviceSN = std::string(snElem->valuestring);
        }
        cJSON *deviceConfigElem = cJSON_GetObjectItem(deviceElem, "syncConfig");
        if(cJSON_IsObject(deviceConfigElem)) {
            cJSON *numberElem = nullptr;
            cJSON *strElem    = nullptr;
            cJSON *bElem      = nullptr;
            strElem           = cJSON_GetObjectItemCaseSensitive(deviceConfigElem, "syncMode");
            if(cJSON_IsString(strElem) && strElem->valuestring != nullptr) {
                devConfigInfo->syncConfig.syncMode = stringToOBSyncMode(strElem->valuestring);
                std::cout << "config[" << (deviceCount++) << "]: SN=" << std::string(devConfigInfo->deviceSN) << ", mode=" << strElem->valuestring << std::endl;
            }
            numberElem = cJSON_GetObjectItemCaseSensitive(deviceConfigElem, "depthDelayUs");
            if(cJSON_IsNumber(numberElem)) {
                devConfigInfo->syncConfig.depthDelayUs = numberElem->valueint;
            }
            numberElem = cJSON_GetObjectItemCaseSensitive(deviceConfigElem, "colorDelayUs");
            if(cJSON_IsNumber(numberElem)) {
                devConfigInfo->syncConfig.colorDelayUs = numberElem->valueint;
            }
            numberElem = cJSON_GetObjectItemCaseSensitive(deviceConfigElem, "trigger2ImageDelayUs");
            if(cJSON_IsNumber(numberElem)) {
                devConfigInfo->syncConfig.trigger2ImageDelayUs = numberElem->valueint;
            }
            numberElem = cJSON_GetObjectItemCaseSensitive(deviceConfigElem, "triggerOutDelayUs");
            if(cJSON_IsNumber(numberElem)) {
                devConfigInfo->syncConfig.triggerOutDelayUs = numberElem->valueint;
            }
            bElem = cJSON_GetObjectItemCaseSensitive(deviceConfigElem, "triggerOutEnable");
            if(cJSON_IsBool(bElem)) {
                devConfigInfo->syncConfig.triggerOutEnable = (bool)bElem->valueint;
            }
            bElem = cJSON_GetObjectItemCaseSensitive(deviceConfigElem, "framesPerTrigger");
            if(cJSON_IsNumber(bElem)) {
                devConfigInfo->syncConfig.framesPerTrigger = bElem->valueint;
            }
        }

        if(OB_MULTI_DEVICE_SYNC_MODE_FREE_RUN != devConfigInfo->syncConfig.syncMode) {
            deviceConfigList.push_back(devConfigInfo);
        }
        else {
            std::cerr << "Invalid sync mode of deviceSN: " << devConfigInfo->deviceSN << std::endl;
        }
        devConfigInfo = nullptr;
    }

    // 2. Get sensor control config
    {
        cJSON *sensorControlElem = cJSON_GetObjectItem(rootElem, "sensorControl");
        cJSON *strElem           = nullptr;
        cJSON *bElem             = nullptr;
        cJSON *intElem           = nullptr;
        if(sensorControlElem) {
            sensorControlInfo = std::shared_ptr<SensorControlInfo>(new SensorControlInfo{});
            // sensor control switch
            bElem = cJSON_GetObjectItemCaseSensitive(sensorControlElem, "enable");
            if(cJSON_IsBool(bElem)) {
                sensorControlInfo->enable = bElem->valueint;
            }
            // depth preset
            strElem = cJSON_GetObjectItemCaseSensitive(sensorControlElem, "depthPreset");
            if(cJSON_IsString(strElem) && strElem->valuestring != nullptr) {
                sensorControlInfo->depthPreset = std::string(strElem->valuestring);
            }

            // laser power level
            intElem = cJSON_GetObjectItemCaseSensitive(sensorControlElem, "laserPowerLevel");
            if(cJSON_IsNumber(intElem)) {
                sensorControlInfo->laserPowerLevel = intElem->valueint;
            }
            // disparity search range
            std::map<std::string, int> disparitySearchRangeMap = { { "64", 0 }, { "128", 1 }, { "256", 2 } };
            strElem                                            = cJSON_GetObjectItemCaseSensitive(sensorControlElem, "disparitySearchRange");
            if(cJSON_IsString(strElem) && strElem->valuestring != nullptr) {
                sensorControlInfo->disparitySearchRange = disparitySearchRangeMap[strElem->valuestring];
            }

            // hardware d2d
            bElem = cJSON_GetObjectItemCaseSensitive(sensorControlElem, "hardwareD2d");
            if(cJSON_IsBool(bElem)) {
                sensorControlInfo->hardwareD2d = bElem->valueint;
            }

            // hardware noise removal
            bElem = cJSON_GetObjectItemCaseSensitive(sensorControlElem, "hardwareNoiseRemoval");
            if(cJSON_IsBool(bElem)) {
                sensorControlInfo->hardwareNoiseRemoval = bElem->valueint;
            }

            // software noise removal
            bElem = cJSON_GetObjectItemCaseSensitive(sensorControlElem, "softwareNoiseRemoval");
            if(cJSON_IsBool(bElem)) {
                sensorControlInfo->softwareNoiseRemoval = bElem->valueint;
            }

            // laser on
            bElem = cJSON_GetObjectItemCaseSensitive(sensorControlElem, "laserOn");
            if(cJSON_IsBool(bElem)) {
                sensorControlInfo->laserOn = bElem->valueint;
            }

            // color auto exposure
            bElem = cJSON_GetObjectItemCaseSensitive(sensorControlElem, "colorAutoExposure");
            if(cJSON_IsBool(bElem)) {
                sensorControlInfo->colorAutoExposure = bElem->valueint;
            }

            // color exposure and gain
            intElem = cJSON_GetObjectItemCaseSensitive(sensorControlElem, "colorExposure");
            if(cJSON_IsNumber(intElem)) {
                sensorControlInfo->colorExposure = intElem->valueint;
            }

            intElem = cJSON_GetObjectItemCaseSensitive(sensorControlElem, "colorGain");
            if(cJSON_IsNumber(intElem)) {
                sensorControlInfo->colorGain = intElem->valueint;
            }

            // ir AE
            bElem = cJSON_GetObjectItemCaseSensitive(sensorControlElem, "irAutoExposure");
            if(cJSON_IsBool(bElem)) {
                sensorControlInfo->irAutoExposure = bElem->valueint;
            }

            // ir exposure and gain
            intElem = cJSON_GetObjectItemCaseSensitive(sensorControlElem, "irExposure");
            if(cJSON_IsNumber(intElem)) {
                sensorControlInfo->irExposure = intElem->valueint;
            }

            intElem = cJSON_GetObjectItemCaseSensitive(sensorControlElem, "irGain");
            if(cJSON_IsNumber(intElem)) {
                sensorControlInfo->irGain = intElem->valueint;
            }
        }
    }
    // 3. Get sensor config
    cJSON *streamProfileElem = cJSON_GetObjectItem(rootElem, "streamProfile");
    if(streamProfileElem) {
        std::vector<std::string>            sensorNames   = { "color", "depth" };
        std::map<std::string, OBSensorType> sensorTypeMap = { { "color", OB_SENSOR_COLOR }, { "depth", OB_SENSOR_DEPTH } };
        for(const std::string &sensorName: sensorNames) {
            auto profileConfig = parseStreamProfileConfig(streamProfileElem, sensorName);
            if(profileConfig) {
                profileConfig->sensorType = sensorTypeMap[sensorName];
                streamConfigList.push_back(profileConfig);
            }
        }
    }

    cJSON_Delete(rootElem);
    return true;
}

OBMultiDeviceSyncMode stringToOBSyncMode(const std::string &modeString) {
    static const std::unordered_map<std::string, OBMultiDeviceSyncMode> syncModeMap = {
        { "OB_MULTI_DEVICE_SYNC_MODE_FREE_RUN", OB_MULTI_DEVICE_SYNC_MODE_FREE_RUN },
        { "OB_MULTI_DEVICE_SYNC_MODE_STANDALONE", OB_MULTI_DEVICE_SYNC_MODE_STANDALONE },
        { "OB_MULTI_DEVICE_SYNC_MODE_PRIMARY", OB_MULTI_DEVICE_SYNC_MODE_PRIMARY },
        { "OB_MULTI_DEVICE_SYNC_MODE_SECONDARY", OB_MULTI_DEVICE_SYNC_MODE_SECONDARY },
        { "OB_MULTI_DEVICE_SYNC_MODE_SECONDARY_SYNCED", OB_MULTI_DEVICE_SYNC_MODE_SECONDARY_SYNCED },
        { "OB_MULTI_DEVICE_SYNC_MODE_SOFTWARE_TRIGGERING", OB_MULTI_DEVICE_SYNC_MODE_SOFTWARE_TRIGGERING },
        { "OB_MULTI_DEVICE_SYNC_MODE_HARDWARE_TRIGGERING", OB_MULTI_DEVICE_SYNC_MODE_HARDWARE_TRIGGERING }
    };
    auto it = syncModeMap.find(modeString);
    if(it != syncModeMap.end()) {
        return it->second;
    }
    // Constructing exception messages with stringstream
    std::stringstream ss;
    ss << "Unrecognized sync mode: " << modeString;
    throw std::invalid_argument(ss.str());
}

std::string OBSyncModeToString(const OBMultiDeviceSyncMode syncMode) {
    static const std::unordered_map<OBMultiDeviceSyncMode, std::string> modeToStringMap = {
        { OB_MULTI_DEVICE_SYNC_MODE_FREE_RUN, "OB_MULTI_DEVICE_SYNC_MODE_FREE_RUN" },
        { OB_MULTI_DEVICE_SYNC_MODE_STANDALONE, "OB_MULTI_DEVICE_SYNC_MODE_STANDALONE" },
        { OB_MULTI_DEVICE_SYNC_MODE_PRIMARY, "OB_MULTI_DEVICE_SYNC_MODE_PRIMARY" },
        { OB_MULTI_DEVICE_SYNC_MODE_SECONDARY, "OB_MULTI_DEVICE_SYNC_MODE_SECONDARY" },
        { OB_MULTI_DEVICE_SYNC_MODE_SECONDARY_SYNCED, "OB_MULTI_DEVICE_SYNC_MODE_SECONDARY_SYNCED" },
        { OB_MULTI_DEVICE_SYNC_MODE_SOFTWARE_TRIGGERING, "OB_MULTI_DEVICE_SYNC_MODE_SOFTWARE_TRIGGERING" },
        { OB_MULTI_DEVICE_SYNC_MODE_HARDWARE_TRIGGERING, "OB_MULTI_DEVICE_SYNC_MODE_HARDWARE_TRIGGERING" }
    };

    auto it = modeToStringMap.find(syncMode);
    if(it != modeToStringMap.end()) {
        return it->second;
    }
    std::stringstream ss;
    ss << "Unmapped sync mode value: " << static_cast<int>(syncMode) << ". Please check the sync mode value.";
    throw std::invalid_argument(ss.str());
}

int strcmp_nocase(const char *str0, const char *str1) {
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    return _strcmpi(str0, str1);
#else
    return strcasecmp(str0, str1);
#endif
}

OBFrameType mapFrameType(OBSensorType sensorType) {
    switch(sensorType) {
    case OB_SENSOR_COLOR:
        return OB_FRAME_COLOR;
    case OB_SENSOR_IR:
        return OB_FRAME_IR;
    case OB_SENSOR_IR_LEFT:
        return OB_FRAME_IR_LEFT;
    case OB_SENSOR_IR_RIGHT:
        return OB_FRAME_IR_RIGHT;
    case OB_SENSOR_DEPTH:
        return OB_FRAME_DEPTH;
    default:
        return OBFrameType::OB_FRAME_UNKNOWN;
    }
}

std::ostream &operator<<(std::ostream &os, const PipelineHolder &holder) {
    if(os.good()) {
        os << "deviceSN: " << holder.deviceSN << ", sensorType: ";
        switch(holder.sensorType) {
        case OB_SENSOR_COLOR:
            os << "OB_SENSOR_COLOR";
            break;
        case OB_SENSOR_DEPTH:
            os << "OB_SENSOR_DEPTH";
            break;
        default:
            os << static_cast<int>(holder.sensorType);
            break;
        }
        os << ", deviceIndex: " << holder.deviceIndex;
    }
    return os;
}
