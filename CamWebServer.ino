/*
CamWebServer.ino

A modification of the DFR1154 sample script.

Description: Creates a webcam port running from the DFROBOT ESP32-S3 AI Camera V1.0

Arduino Tool Configs:
  - USB CDC on Boot: Enabled
  - CPU Frequency: 240 MHz (Wifi)
  - Flash Mode: QIO 80 MHz
  - Flash Size: 16 MB, (8 MB should also work)
  - PSRAM: OPI PSRAM
  - Partition Scheme: Huge App (3MB No OTA...)
  
NOTE: This has been verified to work on Edge Browser and not Chrome.
*/
#include <WiFi.h>
#include "esp_camera.h"
#include "camera_index.h"
#include <Preferences.h>

#define PWDN_GPIO_NUM     -1
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM     5
#define Y9_GPIO_NUM       4
#define Y8_GPIO_NUM       6
#define Y7_GPIO_NUM       7
#define Y6_GPIO_NUM       14
#define Y5_GPIO_NUM       17
#define Y4_GPIO_NUM       21
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM       16
#define VSYNC_GPIO_NUM    1
#define HREF_GPIO_NUM     2
#define PCLK_GPIO_NUM     15
#define SIOD_GPIO_NUM  8
#define SIOC_GPIO_NUM  9

#define LED_GPIO_NUM  47

Preferences preferences;
const char* const ssid = "****";
const char* const password = "****";

static unsigned long lastCheck = 0;

void startCameraServer();
void setupLedFlash(int pin);

void connectToWiFi(const char* ssid, const char* password) {
  WiFi.begin(ssid, password);
  WiFi.setTxPower(WIFI_POWER_2dBm); // Reduces current rating when searching for wifi: fix for inconsistent wifi connections status
  Serial.begin(115200);
  Serial.printf("Connecting to WiFi: %s\n", ssid);
  int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < 40) {
    delay(500);
    Serial.print(".");
    retries++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nFailed to connect to WiFi.");
  }
}


void initWiFi() {
  preferences.begin("wifi", false);
  String savedSSID = preferences.getString("ssid", "");
  String savedPASS = preferences.getString("password", "");

  if (savedSSID.length() > 0 && savedPASS.length() > 0) {
    Serial.println("Found saved WiFi credentials.");
    connectToWiFi(savedSSID.c_str(), savedPASS.c_str());

    if (WiFi.status() == WL_CONNECTED) {
      preferences.end();
      return;
    }
  }

  Serial.println("Enter SSID: ");
  while (Serial.available() == 0) delay(10);
  String inputSSID = Serial.readStringUntil('\n');
  inputSSID.trim();

  Serial.println("Enter Password: ");
  while (Serial.available() == 0) delay(10);
  String inputPASS = Serial.readStringUntil('\n');
  inputPASS.trim();

  connectToWiFi(inputSSID.c_str(), inputPASS.c_str());

  if (WiFi.status() == WL_CONNECTED) {
    preferences.putString("ssid", inputSSID);
    preferences.putString("password", inputPASS);
    Serial.println("WiFi credentials saved.");
  }

  preferences.end();
}

void initCamera(){
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAMESIZE_VGA;
  config.pixel_format = PIXFORMAT_JPEG;  // for streaming
  //config.pixel_format = PIXFORMAT_RGB565; // for face detection/recognition
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count = 1;

 // if PSRAM IC present, init with higher quality
  if (config.pixel_format == PIXFORMAT_JPEG) {
    if (psramFound()) {
      Serial.println("PSRAM found!");
      config.jpeg_quality = 10;
      config.fb_count = 2;
      config.grab_mode = CAMERA_GRAB_LATEST;
    } else {
      Serial.println("PSRAM NOT found - using DRAM");
      config.frame_size = FRAMESIZE_SVGA;
      config.fb_location = CAMERA_FB_IN_DRAM;
    }
  }

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x\n", err);
    return;
  }
  
  Serial.println("Camera initialized successfully");

  // Get sensor and configure it
  //@Error_catch: Bad camera sensor
  sensor_t *s = esp_camera_sensor_get();
  if (s == NULL) {
    Serial.println("ERROR: Camera sensor not found!");
    return;
  }
  
  Serial.printf("Camera sensor PID: 0x%02X\n", s->id.PID);

  // Configure sensor based on type
  //@Error_catch: Wrong camera config
  if (s->id.PID == OV3660_PID) {
    Serial.println("Detected OV3660 sensor");
    s->set_vflip(s, 1);        // flip it back
    s->set_brightness(s, 1);   // up the brightness just a bit
    s->set_saturation(s, -2);  // lower the saturation
  } else if (s->id.PID == OV5640_PID) {
    Serial.println("Detected OV5640 sensor");
    s->set_vflip(s, 1);
  } else if (s->id.PID == OV2640_PID) {
    Serial.println("Detected OV2640 sensor");
  } else {
    Serial.printf("Unknown camera sensor: 0x%02X\n", s->id.PID);
  }

  // Set frame size for streaming
  s->set_framesize(s, FRAMESIZE_QVGA);
  
  Serial.println("Camera configuration complete");
}

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  delay(2000);  
  Serial.println("\n\n=== ESP32-S3 AI Camera Web Server ===\n");
  
  initCamera();
  setupLedFlash(LED_GPIO_NUM);
  initWiFi();
  startCameraServer();
  
  Serial.println(" Setup complete!");
}

void loop() {
  //@Error_catch: Proof of image capture
  // Every 5 seconds, try to capture a frame and report status
  if (millis() - lastCheck > 5000) {
    lastCheck = millis();
    
    camera_fb_t *fb = esp_camera_fb_get();
    if (fb) {
      Serial.printf("Frame captured - Size: %d bytes, Width: %d, Height: %d\n", 
                    fb->len, fb->width, fb->height);
      esp_camera_fb_return(fb);
    } else {
      Serial.println("Failed to capture frame - Camera busy or error");
    }
  }
  
  delay(100);
}
