#include "esp_camera.h"
#include <WiFi.h>
#include <string.h> 
#include <HTTPClient.h>
#include <SPIFFS.h>
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

extern int ble_state; 
extern int wifi_state; 
extern int wifi_ble_switch_cmd_enable;

void startCameraServer();
void init_camera(); 
void initSPIFFS(); 
void BleServer_init();
void receiveAndNotify(); 
void init_wifi_sta(); 
void stop_wifi(); 
void start_ble(); 
void restart_ble(); 
void debug_ble_start(); 
void stop_ble(); 
void start_ad(); 
void stop_ad(); 
void restart_ble_init_enable(); 
void stop_ble_deinit_disable(); 
void clearall_ble(); 

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  // Ble Server init
  start_ble(); 

  // init spiff 
  initSPIFFS(); 

  // init Wifi 
  init_wifi_sta(); 

  // init camera 
  init_camera(); 

  startCameraServer();

  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");

  wifi_ble_switch_cmd_enable = 1; 

#ifdef ARDUINO_ARCH_ESP32
  Serial.println("ARDUINO_ARCH_ESP32"); 
#else 
  Serial.println("NO ARDUINO_ARCH_ESP32"); 
#endif 

#ifdef CLASSIC_BT_ENABLED
  Serial.println("CLASSIC_BT_ENABLED"); 
#else 
  Serial.println("NO CLASSIC_BT_ENABLED"); 
#endif 

}

void loop() {
  // only wifi_ble_switch_cmd is enabled, the command will be valid. Otherwise, the command will be received in app_httpd.cpp to control taking pictures
  if (wifi_ble_switch_cmd_enable == 1 && Serial.available()) {
    String s = Serial.readStringUntil('\n');

    if (s == "wifi stop" && wifi_state == 1) {
      Serial.println("[INFO] Wifi stop cmd received."); 
      stop_wifi(); 
    }
    else if (s == "wifi start" && wifi_state == 0) {
      Serial.println("[INFO] Wifi start cmd received."); 
      init_wifi_sta(); 
    }

    else if (s == "ble stop" && ble_state == 1) {
      Serial.println("[INFO] BLE stop cmd received."); 
      stop_ble(); 
    }
    else if (s == "ble start" && ble_state == 0) {
      Serial.println("[INFO] BLE start cmd received."); 
      //start_ble(); 
      restart_ble(); 
    }

    else if (s == "status") {
      Serial.printf("[INFO] status - wifi: %d, ble: %d\n", wifi_state, ble_state);
    }

    else if (s == "stop ad" && ble_state == 1) {
      Serial.println("[INFO] AD stop cmd received."); 
      stop_ad(); 
    }
    else if (s == "start ad" && ble_state == 1) {
      Serial.println("[INFO] AD start cmd received."); 
      start_ad(); 
    }

    else if (s == "ble init" && ble_state == 0) {
      Serial.println("[INFO] BLE init cmd received."); 
      restart_ble_init_enable(); 
    }
    else if (s == "ble deinit" && ble_state == 1) {
      Serial.println("[INFO] BLE deinit cmd received."); 
      stop_ble_deinit_disable(); 
    }

    else if (s == "ble startover" && ble_state == 0) {
      Serial.println("[INFO] BLE startover cmd received."); 
      start_ble(); 
    }
    else if (s == "ble clearall" && ble_state == 1) {
      Serial.println("[INFO] BLE clearall cmd received."); 
      clearall_ble(); 
    }

    else {
      Serial.print("[INFO] Invalid cmd: "); 
      Serial.println(s);  
    }

    Serial.print("[DEBUG] Free heap memory: ");
    Serial.println(ESP.getFreeHeap());
  }

  if (ble_state == 1) {
    receiveAndNotify(); 
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

void init_wifi_sta() {
  WiFi.begin(ssid, password);
  WiFi.setSleep(true);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  wifi_state = 1; 
}

void stop_wifi() {
  if (WiFi.mode(WIFI_OFF)) {
    Serial.println("[INFO] Wifi stop succeeded."); 
    wifi_state = 0; 
  } 
  else {
    Serial.println("Wifi stop failed."); 
  }
}


