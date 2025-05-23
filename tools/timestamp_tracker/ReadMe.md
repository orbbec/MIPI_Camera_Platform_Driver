# Camera Timestamp Tracker Tool

## Background

Frame rate and timestamp stability are two important performance indicators for cameras. We need a tool that can record frame rate and timestamps in order to analyze their stability.

## Features

The tool mainly supports the following features:
1. Load the first connected camera device on the system and start all supported data streams;
2. Capture each frame from the data streams and save frame rate, timestamp, and other information into a CSV file. The CSV files are saved in the `output` directory under the tool’s folder. The filename format is:  
   `{DeviceName}.{DeviceSerial}.{Sensor}.{Resolution}.{FrameRate}fps.{Timestamp}.csv`  
   For example:  
   `Orbbec Gemini 335Lg.CP1S34D0002D.COLOR.848x480.30fps.20250517164457.csv`
3. Frame rate monitoring: The tool periodically analyzes and prints the frame rate of the data streams, allowing you to directly detect any dropped frames.
4. Supported devices: Various cameras supported by the OpenOrbbecSDK.

## Usage

### Launch

Double-click the `timestamp_tracker` executable file to start the tool. By default, it captures up to 20,000 frames.  
Alternatively, run the tool in a terminal and specify the number of frames to capture:  
`./timestamp_tracker -n 50000`

### Exit

1. The program exits automatically after capturing the specified number of frames;
2. Or press the `Esc` key in the window, or close the window directly to exit the program.

## Data Analysis

TODO
