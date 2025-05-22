# ORBBEC Camera Driver for GMSL Interface

## Introduction to GMSL Camera Driver
This repository contains driver files for Orbbec GMSL interface cameras, designed to facilitate integration of GMSL interface cameras for developers and customers.

### Directory Structure
- **bin**: Pre-built binary files for Orbbec GMSL cameras; ready for direct integration.
- **doc**: Documentation related to GMSL integration, primarily in PDF format.
- **src**: 
  - **kernel**: Source code for driver adaptation.
  - **patch**: Patch files for specific platforms.
- **tools**: Demo examples for validating GMSL functionality.

### Branch Description
- **main**: Stable release branch containing tested code and binary files.
- **gemini-335Lg**: Development branch for Gemini-335Lg camera adaptation.
- **gemini-345lg**: Development branch for Gemini-345Lg camera adaptation.

### GMSL Integration Workflow
![](./doc/image/flowchart.png)

## Supported Cameras and Platforms

### Supported Cameras
- Gemini-335Lg
- Gemini-345Lg

### Supported Platforms
- Jetson AGX Orin DevKit
  - Jetpack 6.0 (r36_release_v3.0)
  - Jetpack 6.1 (r36_release_v4.0)
  - Jetpack 6.2 (r36_release_v4.3)
- MIC-733-AO
  - Jetpack 6.0 (r36_release_v3.0)
  - Jetpack 6.1 (r36_release_v4.0)

## GMSL Integration Procedures

### Hardware Integration Requirements


### Software Driver Installation
- Binary (bin) driver installation
- Patch-based driver installation
- Kernel source code installation

## GMSL Camera Performance Metrics
After successful GMSL camera integration, the following performance metrics should be monitored:
- Timestamp stability
- Frame rate stability
- Synchronization accuracy
For testing methodology and tools, please refer to [Performance Test Guide.pdf](./doc/Performance Test Guide.pdf).

## Usage Limitations of GMSL Cameras
GMSL cameras interface with various deserializer chips such as MAX9296 and MAX92716. Orbbec GMSL cameras support multiple streams including depth, color, IR, and IMU data, but certain usage limitations apply:
- GMSL only supports V4L2 and YUYV format; MJPG format is not supported. RGB output is derived from YUYV format conversion.
- Metadata for Gemini-335Lg is provided via a separate node, while metadata for other models is embedded within video frames, which remains transparent to users.
- Two cameras connected on the same MAX9296, MAX96712 LinkA/B, or MAX96712 LinkC/D have the following limitations:
  - RGB stream from one camera cannot coexist with the right IR stream from another camera.
  - Depth stream from one camera cannot coexist with the left IR stream from another camera.
  - The combined maximum number of active streams from both cameras is limited to four (satisfying the above two conditions ensures compliance).

For further known limitations, please refer to [Usage Limitations of Orbbec GMSL Cameras](./doc/Orbbec GMSL Series Camera Usage Limitations.pdf).

## Issue Reporting for GMSL Usage
To streamline issue resolution for developers and customers during GMSL camera integration, we adopt GitHub Issues for tracking feedback due to variations in hardware and software environments. Please follow the guidelines below for issue reporting:

- First check existing issues before submitting a new one.
- If no solution is found, submit a new issue using the provided template to expedite problem diagnosis.

## Frequently Asked Questions (FAQ)
A list of common questions is provided for reference.