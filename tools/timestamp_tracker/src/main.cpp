// Copyright (c) Orbbec Inc. All Rights Reserved.
// Licensed under the MIT License.

#include <libobsensor/ObSensor.h>
#include <libobsensor/ObSensor.hpp>
#include "utils.hpp"

#include <mutex>
#include <thread>
#include <fstream>
#include <iostream>
#include <queue>
#include <condition_variable>

#include <iomanip>
#include <chrono>
#include <map>

#ifdef _WIN32
#include <io.h>
#include <direct.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#include <cstring>
#endif

#define ESC_KEY 27
std::mutex                                frameMutex;
std::queue<std::shared_ptr<ob::FrameSet>> frameSetQueue;
std::condition_variable                   frameCv;

std::mutex                                imuFrameMutex;
std::queue<std::shared_ptr<ob::FrameSet>> imuFrameSetQueue;
std::condition_variable                   imuFrameCv;

std::ofstream colorOutfile;
std::ofstream depthOutfile;
std::ofstream leftIrOutfile;
std::ofstream rightIrOutfile;
std::ofstream accelOutfile;
std::ofstream gyroOutfile;

int colorDiff   = 0;
int depthDiff   = 0;
int leftIrDiff  = 0;
int rightIrDiff = 0;
int accelDiff   = 0;
int gyroDiff    = 0;

struct PreFrameTimeStamp {
    uint64_t preGlobalTimeStamp = 0;
    uint64_t preTimeStamp       = 0;
};

struct StreamStats {
    int frameCount  = 0;
    int expectedFps = 0;
    int lost        = 0;
    int totalLost   = 0;
};

std::map<OBFrameType, StreamStats> streamStats;
std::mutex                         statsMutex;

std::string getFrameTypeName(OBFrameType type) {
    switch(type) {
    case OB_FRAME_COLOR:
        return "Color";
    case OB_FRAME_DEPTH:
        return "Depth";
    case OB_FRAME_IR:
        return "IR";
    case OB_FRAME_IR_LEFT:
        return "IR_Left";
    case OB_FRAME_IR_RIGHT:
        return "IR_Right";
    case OB_FRAME_ACCEL:
        return "Accel";
    case OB_FRAME_GYRO:
        return "Gyro";
    default:
        return "Unknown";
    }
}

int getSampleRate(OBIMUSampleRate rate) {
    switch(rate) {
    case OB_SAMPLE_RATE_25_HZ:
        return 25;
    case OB_SAMPLE_RATE_50_HZ:
        return 50;
    case OB_SAMPLE_RATE_100_HZ:
        return 100;
    case OB_SAMPLE_RATE_200_HZ:
        return 200;
    case OB_SAMPLE_RATE_500_HZ:
        return 500;
    case OB_SAMPLE_RATE_1_KHZ:
        return 1000;
    case OB_SAMPLE_RATE_2_KHZ:
        return 2000;
    default:
        return 0;
    }
}

void startStream(std::shared_ptr<ob::Pipeline> pipe, std::shared_ptr<ob::Pipeline> imuPipe) {

    // Configure which streams to enable or disable for the Pipeline by creating a Config.
    std::shared_ptr<ob::Config> config    = std::make_shared<ob::Config>();
    std::shared_ptr<ob::Config> imuConfig = std::make_shared<ob::Config>();

    // Enumerate and config all sensors.
    auto device = pipe->getDevice();
    // Disable auto exposure
    device->setBoolProperty(OB_PROP_DEPTH_AUTO_EXPOSURE_BOOL, false);
    device->setBoolProperty(OB_PROP_COLOR_AUTO_EXPOSURE_BOOL, false);
    device->setBoolProperty(OB_PROP_IR_AUTO_EXPOSURE_BOOL, false);

    // Get sensor list from device.
    auto sensorList = device->getSensorList();

    for(uint32_t i = 0; i < sensorList->getCount(); i++) {
        // Get sensor type.
        auto sensorType = sensorList->getSensorType(i);

        // exclude gyro and accel sensors.
        if(sensorType == OB_SENSOR_GYRO || sensorType == OB_SENSOR_ACCEL) {
            imuConfig->enableStream(sensorType);
            continue;
        }

        // enable the stream.
        config->enableStream(sensorType);
    }

    // Start the pipeline with config
    pipe->start(config, [&](std::shared_ptr<ob::FrameSet> frameSet) {
        std::lock_guard<std::mutex> lock(frameMutex);
        frameSetQueue.push(frameSet);
        frameCv.notify_all();
    });

    imuPipe->start(imuConfig, [&](std::shared_ptr<ob::FrameSet> frameSet) {
        std::lock_guard<std::mutex> lock(imuFrameMutex);
        imuFrameSetQueue.push(frameSet);
        imuFrameCv.notify_all();
    });
}

void stopStream(std::shared_ptr<ob::Pipeline> pipe, std::shared_ptr<ob::Pipeline> imuPipe) {
    // Stop the pipeline
    pipe->stop();
    imuPipe->stop();

    while(!frameSetQueue.empty()) {
        frameSetQueue.pop();
    }

    while(!imuFrameSetQueue.empty()) {
        imuFrameSetQueue.pop();
    }
}

std::string getFormatTimestamp() {
    auto    currentTime = std::chrono::system_clock::now();
    auto    time_t_time = std::chrono::system_clock::to_time_t(currentTime);
    std::tm tm;
#if defined(_WIN32) || defined(_WIN64)
    localtime_s(&tm, &time_t_time);
#else
    localtime_r(&time_t_time, &tm);
#endif

    std::stringstream ss;
    ss << std::put_time(&tm, "%Y%m%d%H%M%S");
    return ss.str();
}

inline int mkDirs(const char *muldir) {
    int    i;
    size_t len;
    char   str[512];
    int    error = 0;
#ifdef _WIN32
    strncpy_s(str, muldir, 512);
    len = strlen(str);
    for(i = 0; i < len; i++) {
        if(str[i] == '/' || str[i] == '\\') {
            str[i] = '\0';
            if(_access(str, 0) != 0) {
                error = _mkdir(str);
                if(error != 0) {
                    return error;
                }
            }
            str[i] = '/';
        }
    }
    if(len > 0 && _access(str, 0) != 0) {
        error = _mkdir(str);
    }
    return error;
#else
    strncpy(str, muldir, 512);
    len = strlen(str);
    umask(0000);
    for(i = 0; i < len; i++) {
        if(i > 0 && str[i] == '/') {
            str[i] = '\0';
            if(access(str, 0) != 0) {
                error = mkdir(str, 0777);
                if(error != 0) {
                    return error;
                }
            }
            str[i] = '/';
        }
    }
    if(len > 0 && access(str, 0) != 0) {
        error = mkdir(str, 0777);
    }
    return error;
#endif
}

void createCSVFile(std::shared_ptr<ob::Pipeline> pipe, std::shared_ptr<ob::Pipeline> imuPipe) {
    auto timeStamp  = getFormatTimestamp();
    auto device     = pipe->getDevice();
    auto deviceInfo = device->getDeviceInfo();

    auto colorProfile   = pipe->getStreamProfileList(OB_SENSOR_COLOR)->getVideoStreamProfile();
    auto depthProfile   = pipe->getStreamProfileList(OB_SENSOR_DEPTH)->getVideoStreamProfile();
    auto leftIrProfile  = pipe->getStreamProfileList(OB_SENSOR_IR_LEFT)->getVideoStreamProfile();
    auto rightIrProfile = pipe->getStreamProfileList(OB_SENSOR_IR_RIGHT)->getVideoStreamProfile();
    auto accelProfile   = pipe->getStreamProfileList(OB_SENSOR_ACCEL)->getAccelStreamProfile(OB_ACCEL_FS_UNKNOWN, OB_SAMPLE_RATE_UNKNOWN);
    auto gyroProfile    = pipe->getStreamProfileList(OB_SENSOR_GYRO)->getGyroStreamProfile(OB_GYRO_FS_UNKNOWN, OB_SAMPLE_RATE_UNKNOWN);

    // Create and initialize a CSV file
    mkDirs("./Output");
    std::string colorPath = "./Output/" + std::string(deviceInfo->getName()) + "_" + std::string(deviceInfo->getSerialNumber()) + "_COLOR_" + std::to_string(colorProfile->getWidth()) + "X"
                            + std::to_string(colorProfile->getHeight()) + "_" + std::to_string(colorProfile->getFps()) + "_" + timeStamp + ".csv";
    std::string depthPath = "./Output/" + std::string(deviceInfo->getName()) + "_" + std::string(deviceInfo->getSerialNumber()) + "_DEPTH_" + std::to_string(depthProfile->getWidth()) + "X"
                            + std::to_string(depthProfile->getHeight()) + "_" + std::to_string(depthProfile->getFps()) + "_" + timeStamp + ".csv";
    std::string leftIrPath = "./Output/" + std::string(deviceInfo->getName()) + "_" + std::string(deviceInfo->getSerialNumber()) + "_LEFT_IR_" + std::to_string(leftIrProfile->getWidth()) + "X"
                             + std::to_string(leftIrProfile->getHeight()) + "_" + std::to_string(leftIrProfile->getFps()) + "_" + timeStamp + ".csv";
    std::string rightIrPath = "./Output/" + std::string(deviceInfo->getName()) + "_" + std::string(deviceInfo->getSerialNumber()) + "_RIGHT_IR_" + std::to_string(rightIrProfile->getWidth()) + "X"
                              + std::to_string(rightIrProfile->getHeight()) + "_" + std::to_string(rightIrProfile->getFps()) + "_" + timeStamp + ".csv";
    std::string accelPath = "./Output/" + std::string(deviceInfo->getName()) + "_" + std::string(deviceInfo->getSerialNumber()) + "_ACCEL_" + std::to_string(getSampleRate(accelProfile->sampleRate()))
                            + "Hz_" + timeStamp + ".csv";
    std::string gyroPath = "./Output/" + std::string(deviceInfo->getName()) + "_" + std::string(deviceInfo->getSerialNumber()) + "_GYRO_" + std::to_string(getSampleRate(gyroProfile->sampleRate()))
                           + "Hz_" + timeStamp + ".csv";
    colorOutfile.open(colorPath);
    if(colorOutfile.is_open()) {
        colorOutfile.clear();
        colorOutfile << "Number,systemTimeStampUs,globalTimeStampUs,globalTimeStampUsDiff,timeStampUs,timeStampUsDiff" << std::endl;
    }

    depthOutfile.open(depthPath);
    if(depthOutfile.is_open()) {
        depthOutfile.clear();
        depthOutfile << "Number,systemTimeStampUs,globalTimeStampUs,globalTimeStampUsDiff,timeStampUs,timeStampUsDiff" << std::endl;
    }

    leftIrOutfile.open(leftIrPath);
    if(leftIrOutfile.is_open()) {
        leftIrOutfile.clear();
        leftIrOutfile << "Number,systemTimeStampUs,globalTimeStampUs,globalTimeStampUsDiff,timeStampUs,timeStampUsDiff" << std::endl;
    }

    rightIrOutfile.open(rightIrPath);
    if(rightIrOutfile.is_open()) {
        rightIrOutfile.clear();
        rightIrOutfile << "Number,systemTimeStampUs,globalTimeStampUs,globalTimeStampUsDiff,timeStampUs,timeStampUsDiff" << std::endl;
    }

    accelOutfile.open(accelPath);
    if(accelOutfile.is_open()) {
        accelOutfile.clear();
        accelOutfile << "Number,systemTimeStampUs,globalTimeStampUs,globalTimeStampUsDiff,timeStampUs,timeStampUsDiff" << std::endl;
    }

    gyroOutfile.open(gyroPath);
    if(gyroOutfile.is_open()) {
        gyroOutfile.clear();
        gyroOutfile << "Number,systemTimeStampUs,globalTimeStampUs,globalTimeStampUsDiff,timeStampUs,timeStampUsDiff" << std::endl;
    }

    {
        std::lock_guard<std::mutex> lock(statsMutex);
        streamStats[OB_FRAME_COLOR].expectedFps    = colorProfile->fps();
        streamStats[OB_FRAME_DEPTH].expectedFps    = depthProfile->fps();
        streamStats[OB_FRAME_IR_LEFT].expectedFps  = leftIrProfile->fps();
        streamStats[OB_FRAME_IR_RIGHT].expectedFps = rightIrProfile->fps();
        streamStats[OB_FRAME_ACCEL].expectedFps    = getSampleRate(accelProfile->sampleRate());
        streamStats[OB_FRAME_GYRO].expectedFps     = getSampleRate(gyroProfile->sampleRate());
    }

    {
        colorDiff   = (1000 / colorProfile->fps()) * 2;
        depthDiff   = (1000 / depthProfile->fps()) * 2;
        leftIrDiff  = (1000 / leftIrProfile->fps()) * 2;
        rightIrDiff = (1000 / rightIrProfile->fps()) * 2;
        accelDiff   = (1000 / getSampleRate(accelProfile->sampleRate())) * 2;
        gyroDiff    = (1000 / getSampleRate(gyroProfile->sampleRate())) * 2;
    }
}

void processFrame(OBFrameType type, bool lostFlag) {
    std::lock_guard<std::mutex> lock(statsMutex);
    streamStats[type].frameCount++;
    if(lostFlag)
        streamStats[type].lost++;
}

int main(void) try {
    // Create a context.
    ob::Context ctx;
    ctx.enableDeviceClockSync(360000);

    // Create a pipeline with default device.
    std::shared_ptr<ob::Pipeline> pipe    = std::make_shared<ob::Pipeline>();
    std::shared_ptr<ob::Pipeline> imuPipe = std::make_shared<ob::Pipeline>();

    startStream(pipe, imuPipe);
    bool isStopStream = false;
    // Record the timestamp of the previous frame
    PreFrameTimeStamp preColorFrame;
    PreFrameTimeStamp preDepthFrame;
    PreFrameTimeStamp preLeftIrFrame;
    PreFrameTimeStamp preRightIrFrame;
    PreFrameTimeStamp preAccelFrame;
    PreFrameTimeStamp preGyroFrame;

    createCSVFile(pipe, imuPipe);
    auto videoFrameThread = std::thread([&] {
        while(!isStopStream) {

            std::shared_ptr<ob::FrameSet> frameset = nullptr;
            {
                std::unique_lock<std::mutex> lock(frameMutex);
                frameCv.wait_for(lock, std::chrono::milliseconds(100), [] { return !frameSetQueue.empty(); });
                if(frameSetQueue.empty()) {
                    continue;
                }
                frameset = frameSetQueue.front();
                frameSetQueue.pop();
            }

            auto colorFrame = frameset->colorFrame();
            if(colorFrame) {
                auto     index                   = colorFrame->index();
                auto     systemTimeStamp         = colorFrame->systemTimeStampUs();
                auto     globalTimestamp         = colorFrame->globalTimeStampUs();
                auto     devTimestamp            = colorFrame->timeStampUs();
                uint64_t globalTimestampDiff     = (preColorFrame.preGlobalTimeStamp == 0) ? (0) : (globalTimestamp - preColorFrame.preGlobalTimeStamp) / 1000;
                preColorFrame.preGlobalTimeStamp = globalTimestamp;

                uint64_t devTimestampDiff  = (preColorFrame.preTimeStamp == 0) ? (0) : (devTimestamp - preColorFrame.preTimeStamp) / 1000;
                preColorFrame.preTimeStamp = devTimestamp;
                // std::cout << "Color frame index: " << index << " , systemTimeStamp: " << systemTimeStamp << ", globalTimeStamp: " << globalTimestamp << " , globalTimeStampDiff: " <<
                // globalTimestampDiff << " ms , timeStamp: " <<  devTimestamp << " , timeStampDiff: " << devTimestampDiff << " ms" << std::endl;
                colorOutfile << index << "," << systemTimeStamp << "," << globalTimestamp << "," << globalTimestampDiff << "," << devTimestamp << "," << devTimestampDiff << std::endl;
                (devTimestampDiff >= colorDiff) ? processFrame(OB_FRAME_COLOR, true) : processFrame(OB_FRAME_COLOR, false);
            }

            auto depthFrame = frameset->depthFrame();
            if(depthFrame) {
                auto     index                   = depthFrame->index();
                auto     systemTimeStamp         = depthFrame->systemTimeStampUs();
                auto     globalTimestamp         = depthFrame->globalTimeStampUs();
                auto     devTimestamp            = depthFrame->timeStampUs();
                uint64_t globalTimestampDiff     = (preDepthFrame.preGlobalTimeStamp == 0) ? (0) : (globalTimestamp - preDepthFrame.preGlobalTimeStamp) / 1000;
                preColorFrame.preGlobalTimeStamp = globalTimestamp;

                uint64_t devTimestampDiff  = (preDepthFrame.preTimeStamp == 0) ? (0) : (devTimestamp - preDepthFrame.preTimeStamp) / 1000;
                preDepthFrame.preTimeStamp = devTimestamp;
                // std::cout << "Depth frame index: " << index << " , systemTimeStamp: " << systemTimeStamp << ", globalTimeStamp: " << globalTimestamp << " , globalTimeStampDiff: " <<
                // globalTimestampDiff << " ms , timeStamp: " <<  devTimestamp << " , timeStampDiff: " << devTimestampDiff << " ms" << std::endl;
                depthOutfile << index << "," << systemTimeStamp << "," << globalTimestamp << "," << globalTimestampDiff << "," << devTimestamp << "," << devTimestampDiff << std::endl;
                (devTimestampDiff >= depthDiff) ? processFrame(OB_FRAME_DEPTH, true) : processFrame(OB_FRAME_DEPTH, false);
            }

            auto leftIrFrame = frameset->getFrame(OB_FRAME_IR_LEFT);
            if(leftIrFrame) {
                auto     index                    = leftIrFrame->index();
                auto     systemTimeStamp          = leftIrFrame->systemTimeStampUs();
                auto     globalTimestamp          = leftIrFrame->globalTimeStampUs();
                auto     devTimestamp             = leftIrFrame->timeStampUs();
                uint64_t globalTimestampDiff      = (preLeftIrFrame.preGlobalTimeStamp == 0) ? (0) : (globalTimestamp - preLeftIrFrame.preGlobalTimeStamp) / 1000;
                preLeftIrFrame.preGlobalTimeStamp = globalTimestamp;

                uint64_t devTimestampDiff   = (preLeftIrFrame.preTimeStamp == 0) ? (0) : (devTimestamp - preLeftIrFrame.preTimeStamp) / 1000;
                preLeftIrFrame.preTimeStamp = devTimestamp;
                // std::cout << "leftIr frame index: " << index << " , systemTimeStamp: " << systemTimeStamp << ", globalTimeStamp: " << globalTimestamp << " , globalTimeStampDiff: " <<
                // globalTimestampDiff << " ms , timeStamp: " <<  devTimestamp << " , timeStampDiff: " << devTimestampDiff << " ms" << std::endl;
                leftIrOutfile << index << "," << systemTimeStamp << "," << globalTimestamp << "," << globalTimestampDiff << "," << devTimestamp << "," << devTimestampDiff << std::endl;
                (devTimestampDiff >= leftIrDiff) ? processFrame(OB_FRAME_IR_LEFT, true) : processFrame(OB_FRAME_IR_LEFT, false);
            }

            auto rightIrFrame = frameset->getFrame(OB_FRAME_IR_RIGHT);
            if(rightIrFrame) {
                auto     index                     = rightIrFrame->index();
                auto     systemTimeStamp           = rightIrFrame->systemTimeStampUs();
                auto     globalTimestamp           = rightIrFrame->globalTimeStampUs();
                auto     devTimestamp              = rightIrFrame->timeStampUs();
                uint64_t globalTimestampDiff       = (preRightIrFrame.preGlobalTimeStamp == 0) ? (0) : (globalTimestamp - preRightIrFrame.preGlobalTimeStamp) / 1000;
                preRightIrFrame.preGlobalTimeStamp = globalTimestamp;

                uint64_t devTimestampDiff    = (preRightIrFrame.preTimeStamp == 0) ? (0) : (devTimestamp - preRightIrFrame.preTimeStamp) / 1000;
                preRightIrFrame.preTimeStamp = devTimestamp;
                // std::cout << "rightIr frame index: " << index << " , systemTimeStamp: " << systemTimeStamp << ", globalTimeStamp: " << globalTimestamp << " , globalTimeStampDiff: " <<
                // globalTimestampDiff << " ms , timeStamp: " <<  devTimestamp << " , timeStampDiff: " << devTimestampDiff << " ms" << std::endl;
                rightIrOutfile << index << "," << systemTimeStamp << "," << globalTimestamp << "," << globalTimestampDiff << "," << devTimestamp << "," << devTimestampDiff << std::endl;
                (devTimestampDiff >= rightIrDiff) ? processFrame(OB_FRAME_IR_RIGHT, true) : processFrame(OB_FRAME_IR_RIGHT, false);
            }
        }
    });

    auto imuFrameThread = std::thread([&] {
        while(!isStopStream) {
            std::shared_ptr<ob::FrameSet> imuFrameset = nullptr;
            {
                std::unique_lock<std::mutex> lock(imuFrameMutex);
                imuFrameCv.wait_for(lock, std::chrono::milliseconds(100), [] { return !imuFrameSetQueue.empty(); });
                if(imuFrameSetQueue.empty()) {
                    continue;
                }
                imuFrameset = imuFrameSetQueue.front();
                imuFrameSetQueue.pop();
            }

            auto accelFrame = imuFrameset->getFrame(OB_FRAME_ACCEL);
            if(accelFrame) {
                auto     index                   = accelFrame->index();
                auto     systemTimeStamp         = accelFrame->systemTimeStampUs();
                auto     globalTimestamp         = accelFrame->globalTimeStampUs();
                auto     devTimestamp            = accelFrame->timeStampUs();
                uint64_t globalTimestampDiff     = (preAccelFrame.preGlobalTimeStamp == 0) ? (0) : (globalTimestamp - preAccelFrame.preGlobalTimeStamp) / 1000;
                preAccelFrame.preGlobalTimeStamp = globalTimestamp;

                uint64_t devTimestampDiff  = (preAccelFrame.preTimeStamp == 0) ? (0) : (devTimestamp - preAccelFrame.preTimeStamp) / 1000;
                preAccelFrame.preTimeStamp = devTimestamp;
                // std::cout << "Accel frame index: " << index << " , systemTimeStamp: " << systemTimeStamp << ", globalTimeStamp: " << globalTimestamp << " , globalTimeStampDiff: " <<
                // globalTimestampDiff << " ms , timeStamp: " <<  devTimestamp << " , timeStampDiff: " << devTimestampDiff << " ms" << std::endl;
                accelOutfile << index << "," << systemTimeStamp << "," << globalTimestamp << "," << globalTimestampDiff << "," << devTimestamp << "," << devTimestampDiff << std::endl;
                (devTimestampDiff >= accelDiff) ? processFrame(OB_FRAME_ACCEL, true) : processFrame(OB_FRAME_ACCEL, false);
            }

            auto gyroFrame = imuFrameset->getFrame(OB_FRAME_GYRO);
            if(gyroFrame) {
                auto     index                  = gyroFrame->index();
                auto     systemTimeStamp        = gyroFrame->systemTimeStampUs();
                auto     globalTimestamp        = gyroFrame->globalTimeStampUs();
                auto     devTimestamp           = gyroFrame->timeStampUs();
                uint64_t globalTimestampDiff    = (preGyroFrame.preGlobalTimeStamp == 0) ? (0) : (globalTimestamp - preGyroFrame.preGlobalTimeStamp) / 1000;
                preGyroFrame.preGlobalTimeStamp = globalTimestamp;

                uint64_t devTimestampDiff = (preGyroFrame.preTimeStamp == 0) ? (0) : (devTimestamp - preGyroFrame.preTimeStamp) / 1000;
                preGyroFrame.preTimeStamp = devTimestamp;
                // std::cout << "Gyro frame index: " << index << " , systemTimeStamp: " << systemTimeStamp << ", globalTimeStamp: " << globalTimestamp << " , globalTimeStampDiff: " <<
                // globalTimestampDiff << " ms , timeStamp: " <<  devTimestamp << " , timeStampDiff: " << devTimestampDiff << " ms" << std::endl;
                gyroOutfile << index << "," << systemTimeStamp << "," << globalTimestamp << "," << globalTimestampDiff << "," << devTimestamp << "," << devTimestampDiff << std::endl;
                (devTimestampDiff >= accelDiff) ? processFrame(OB_FRAME_GYRO, true) : processFrame(OB_FRAME_GYRO, false);
            }
        }
    });

    auto statsThread = std::thread([&] {
        while(!isStopStream) {
            std::this_thread::sleep_for(std::chrono::seconds(1));

            std::lock_guard<std::mutex> lock(statsMutex);
            for(auto &item: streamStats) {
                int expected = item.second.expectedFps;
                int actual   = item.second.frameCount;
                int lost     = item.second.lost;

                std::cout << getFrameTypeName(item.first) << ":      Expected FPS: " << (expected) << "  Avg FPS: " << (actual) << "  Lost/s: " << lost
                          << "  Total Lost: " << (item.second.totalLost += lost) << std::endl;

                item.second.frameCount = 0;
                item.second.lost       = 0;
            }
        }
    });

    while(true) {
        if(ob_smpl::waitForKeyPressed(10) == ESC_KEY) {
            isStopStream = true;
            if(videoFrameThread.joinable()) {
                videoFrameThread.join();
            }
            if(imuFrameThread.joinable()) {
                imuFrameThread.join();
            }
            if(statsThread.joinable()) {
                statsThread.join();
            }
            break;
        }
    }
    colorOutfile.close();
    depthOutfile.close();
    leftIrOutfile.close();
    rightIrOutfile.close();
    accelOutfile.close();
    gyroOutfile.close();
    // Stop the Pipeline, no frame data will be generated.
    stopStream(pipe, imuPipe);

    return 0;
}
catch(ob::Error &e) {
    std::cerr << "function:" << e.getFunction() << "\nargs:" << e.getArgs() << "\nmessage:" << e.what() << "\ntype:" << e.getExceptionType() << std::endl;
    std::cout << "\nPress any key to exit.";
    ob_smpl::waitForKeyPressed();
    exit(EXIT_FAILURE);
}
