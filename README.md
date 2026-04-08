# CamWebServer

A web-based camera streaming application for the DFROBOT ESP32-S3 AI Camera V1.0. Modification of [DFR1154_Example Script](https://github.com/DFRobot/DFR1154_Examples/tree/master/5.3%20CameraWebServer)

## Overview

CamWebServer transforms your ESP32-S3 AI Camera into a networked webcam server. The project creates a web interface that streams live video from the camera, accessible through a compatible web browser over WiFi.

**Note:** This project has been verified to work on **Edge Browser** and not Chrome.

## Features

- Live video streaming over WiFi
- Web-based interface for easy access
- Persistent WiFi credential storage
- Automatic camera sensor detection and configuration
- LED flash support
- Frame capture monitoring and status reporting
- Support for multiple camera sensors (OV3660, OV5640, OV2640)

## Hardware Requirements

- Microcontroller: DFROBOT ESP32-S3 AI Camera V1.0
- USB Cable: For initial programming and configuration
- WiFi Network: 2.4GHz WiFi network (required for operation)

## Arduino Configuration

Before uploading the sketch, configure your Arduino IDE with these settings:

| Setting | Value |
|---------|-------|
| USB CDC on Boot | Enabled |
| CPU Frequency | 240 MHz (WiFi) |
| Flash Mode | QIO 80 MHz |
| Flash Size | 16 MB (8 MB also works) |
| PSRAM | OPI PSRAM |
| Partition Scheme | Huge App (3MB No OTA) |

## Installation & Setup

### 1. Prerequisites
- Arduino IDE installed
- ESP32 board package installed
- DFROBOT ESP32-S3 AI Camera connected via USB

### 2. Upload the Sketch
- Open CamWebServer.ino in Arduino IDE
- Verify the board configuration matches the settings above
- Click Upload

### 3. Configure WiFi Credentials
On first run, the sketch will:
1. Check for saved WiFi credentials in device storage
2. If none found, prompt you via Serial Monitor to enter:
   - SSID: Your WiFi network name
   - Password: Your WiFi network password
3. Save credentials for future boot-ups

To use the Serial Monitor:
- Open: Tools → Serial Monitor
- Set Baud Rate: 115200
- Restart the device if needed

### 4. Access the Webcam
1. Once WiFi is connected, check the Serial Monitor for the IP address
2. Open a web browser (preferably Edge)
3. Navigate to: http://<ESP32_IP_ADDRESS>
4. The live video stream should appear

## GPIO Pin Configuration

The sketch uses the following GPIO pins:

| Function | GPIO Pin |
|----------|----------|
| Camera X-Clock | 5 |
| Camera Y9 | 4 |
| Camera Y8 | 6 |
| Camera Y7 | 7 |
| Camera Y6 | 14 |
| Camera Y5 | 17 |
| Camera Y4 | 21 |
| Camera Y3 | 18 |
| Camera Y2 | 16 |
| Camera VSYNC | 1 |
| Camera HREF | 2 |
| Camera PCLK | 15 |
| Camera SDA (SIOD) | 8 |
| Camera SCL (SIOC) | 9 |
| LED Flash | 47 |

## Camera Sensor Support

The sketch automatically detects and configures your camera sensor:

- OV3660: Vertical flip enabled, brightness increased, saturation lowered
- OV5640: Vertical flip enabled
- OV2640: Standard configuration

## Troubleshooting

### WiFi Connection Issues
Problem: Device cannot find or connect to WiFi

Solution: 
- Check SSID and password are correct
- Verify 2.4GHz band is available (not 5GHz only)
- Try power cycling the device
- Check Serial Monitor for connection attempts

### Camera Not Detected
Problem: "ERROR: Camera sensor not found!"

Solution:
- Verify camera is properly connected to the ESP32-S3
- Check that PSRAM initialization is successful
- Try re-uploading the sketch

### Video Stream Not Loading
Problem: Web page shows no video or connection error

Solution:
- Verify IP address is correct in Serial Monitor
- Try using Edge browser instead of Chrome
- Check that device is on same WiFi network
- Verify firewall isn't blocking port 80

### Low Frame Quality
Problem: Grainy or low-quality video stream

Solution:
- Check PSRAM is detected (should show "PSRAM found!" in Serial Monitor)
- Ensure adequate WiFi signal strength
- Adjust JPEG quality setting in code (lower = better quality but larger files)

## Serial Monitor Output

The sketch provides diagnostic information via Serial Monitor (115200 baud):

```
=== ESP32-S3 AI Camera Web Server ===

PSRAM found!
Camera initialized successfully
Camera sensor PID: 0x3660
Detected OV3660 sensor
Camera configuration complete
Connecting to WiFi: WilliamsS25
.
WiFi connected!
IP address: 192.168.x.x
Setup complete!

Frame captured - Size: 12345 bytes, Width: 320, Height: 240
```

## Default Settings

The current code includes default WiFi credentials for testing. For security:
- Change ssid and password in the code before deployment
- Or use the serial input method to set custom credentials

```cpp
const char* const ssid = "WilliamsS25";
const char* const password = "willhem14";
```

## Project Status

- Verified working on Edge Browser
- Not compatible with Chrome
- PSRAM support for enhanced quality
- Multi-sensor support

## Future Improvements

- Add HTTPS/SSL support for secure streaming
- Implement web interface authentication
- Add motion detection and recording
- Support for resolution adjustment via web interface
- Chrome browser compatibility investigation

## License

Based on DFR1154 sample script by DFROBOT

## Support

For issues or questions:
1. Check the Troubleshooting section above
2. Review Serial Monitor output for error codes
3. Verify all Arduino configuration settings
4. Test with a known-good WiFi network

---

Last Updated: 2026-04-08
