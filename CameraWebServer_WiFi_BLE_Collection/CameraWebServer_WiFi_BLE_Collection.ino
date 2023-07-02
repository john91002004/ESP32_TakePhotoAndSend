#include "esp_timer.h"
#include <string.h> 
#include <HTTPClient.h>

//
// WARNING!!! PSRAM IC required for UXGA resolution and high JPEG quality
//            Ensure ESP32 Wrover Module or other board with PSRAM is selected
//            Partial images will be transmitted if image exceeds buffer size
//
//            You must select partition scheme from the board menu that has at least 3MB APP space.
//            Face Recognition is DISABLED for ESP32 and ESP32-S2, because it takes up from 15
//            seconds to process single frame. Face Detection is ENABLED if PSRAM is enabled as well

#define BLE_SEARCH_TIME_LIMIT 30

int choice_count_wait = 0;
int64_t up_time = 0; 
extern int wifi_ble_switch_cmd_enable; 
extern int ble_state; 
extern int wifi_state; 
extern int site_choice; 
extern int httpd_exit_flag; 

void checkConnection() ;
void startCameraServer();
void init_camera(); 
void initSPIFFS(); 
void BleServer_init();
void receiveAndNotify(int size_choice); 
void init_wifi_sta(); 
void stop_wifi(); 
void start_ble(); 
void stop_ble(); 
int getDeviceConnected() ;
void init_pin(); 
void print_wifi_ready_mes();

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  // init pin_mode 
  init_pin(); 

  // init spiff 
  initSPIFFS(); 

  // init Wifi 
  init_wifi_sta(); 

  // init camera ans start
  init_camera(); 
  startCameraServer();

  print_wifi_ready_mes(); 

  wifi_ble_switch_cmd_enable = 1; 
  up_time = esp_timer_get_time(); 
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
      start_ble(); 
    }
    else if (s == "status") {
      Serial.printf("[INFO] status - wifi: %d, ble: %d\n", wifi_state, ble_state);
    }
    else {
      Serial.print("[INFO] Invalid cmd: "); 
      Serial.println(s);  
    }
    Serial.print("[DEBUG] Free heap memory: ");
    Serial.println(ESP.getFreeHeap());
  }

  if (httpd_exit_flag == 1) {
    log_d("Start to switch wifi to ble."); 
    delay(3); 
    log_d("Stop wifi."); 
    wifi_state = 0; 
    stop_wifi(); 

    delay(300); 
    log_d("Start BLE.");
    ble_state = 1; 
    start_ble();

    delay(3000); // debug

    // wait for ble searching result, at least 1 connection should be built.
    log_d("Check connection until there's at 1 device connected."); 
    int count = 0; 
    while (!getDeviceConnected() && count++ < BLE_SEARCH_TIME_LIMIT) {
      delay(1000); 
    }

    log_d("[FLAG] in main loop, httpd_exit_flag = %d", httpd_exit_flag); 
    log_d("[FLAG] in main loop, site_choice = %d", site_choice); 
    receiveAndNotify(site_choice); 

    log_d("Wait for 5 seconds and reboot...");
    delay(5000);
    ESP.restart();
  }
}
