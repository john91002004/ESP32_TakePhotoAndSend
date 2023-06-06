#include "esp_camera.h"
#include <WiFi.h>
#include <string.h> 
#include <HTTPClient.h>
#include <SPIFFS.h>
#include <BLEDevice.h>
//
// WARNING!!! PSRAM IC required for UXGA resolution and high JPEG quality
//            Ensure ESP32 Wrover Module or other board with PSRAM is selected
//            Partial images will be transmitted if image exceeds buffer size
//
//            You must select partition scheme from the board menu that has at least 3MB APP space.
//            Face Recognition is DISABLED for ESP32 and ESP32-S2, because it takes up from 15
//            seconds to process single frame. Face Detection is ENABLED if PSRAM is enabled as well

// ===================
// Select camera model
// ===================
#define CAMERA_MODEL_AI_THINKER  // Has PSRAM
#include "camera_pins.h"  

// ===========================
// Enter your WiFi credentials
// ===========================
const char* ssid = "進擊的巨人";
const char* password = "ContinuousNormalThink";

#define FILE_PHOTO "/photo.jpg"

int ble_state = 0, wifi_state = 0; 

void startCameraServer();
void init_camera(); 
void initSPIFFS(); 
void BleServer_init();
void receiveAndNotify(); 

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  // Ble Server init
  Serial.println("[FLAG] BleServer_init 1"); 
  // Create the BLE Device
  BLEDevice::init("ESP32_notify_server");
  Serial.println("[FLAG] BleServer_init 2"); 
  ble_state = 1; 
  BleServer_init(); 

  // init spiff 
  initSPIFFS(); 

  // init Wifi 
  init_wifi(); 

  // init camera 
  init_camera(); 

  startCameraServer();

  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");

}

void loop() {
  // Do nothing. Everything is done in another task by the web server
  if (Serial.available()) {
    String s = Serial.readStringUntil('\n');
    if (s == "wifi stop" && wifi_state == 1) {
      Serial.println("Wifi stop cmd received."); 
      if (WiFi.mode(WIFI_OFF)) {
        Serial.println("Wifi stop succeeded."); 
        wifi_state = 0; 
      } 
      else {
        Serial.println("Wifi stop failed."); 
      }
    }
    else if (s == "wifi start" && wifi_state == 0) {
      Serial.println("Wifi start cmd received."); 
      WiFi.begin(ssid, password); 
      wifi_state = 1;
    }
    else if (s == "ble stop" && ble_state == 1) {
      Serial.println("BLE stop cmd received."); 
      BLEDevice::deinit();  // didn't release memory
      ble_state = 0; 
    }
    else if (s == "ble start" && ble_state == 0) {
      Serial.println("BLE start cmd received."); 
      BLEDevice::init("ESP32_notify_server");
      ble_state = 1; 
    }
  }
  if (ble_state == 1) {
    receiveAndNotify(); 
    delay(5000); 
  }
}

void initSPIFFS() {
  if (!SPIFFS.begin(true)) {
    Serial.println("[ERROR] An Error has occurred while mounting SPIFFS");
    ESP.restart();
  }
  else {
    delay(500);
    Serial.println("[INFO] SPIFFS mounted successfully");
  }
}

void init_wifi() {
  Serial.println("[FLAG] init_wifi flag 1"); 
  WiFi.begin(ssid, password);
  Serial.println("[FLAG] init_wifi flag 2");
  WiFi.setSleep(true);
  Serial.println("[FLAG] init_wifi flag 3");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  wifi_state = 1; 
}

