#include <SPIFFS.h>
#include <WiFi.h>
#include "esp_camera.h"
#include "app_httpd.h"

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

extern int ble_state; 
extern int wifi_state; 
extern int wifi_ble_switch_cmd_enable;
extern int httpd_exit_flag; 
extern int site_choice; 
extern int pushButton; 
extern int LED_RED; 
extern int LED_GREEN; 
extern int LED_BLUE; 

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

  int off_count = 0; 

  while (WiFi.status() != WL_CONNECTED) {
    if (++off_count > 20) {
      ESP.restart(); 
    }
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

void init_pin() {
  pinMode(pushButton, INPUT); 
  pinMode(LED_RED, OUTPUT); 
  pinMode(LED_GREEN, OUTPUT); 
  pinMode(LED_BLUE, OUTPUT); 
} 

int get_controller_site(int choice) {
  delay(200); 
  int buttonState = digitalRead(pushButton);
  if (buttonState == HIGH) {
    if (++choice > 3) {
      choice = 1; 
    }
  }
  return choice; 
}

void display_site_LED(int choice) {
  if (choice == 1) {
    digitalWrite(LED_RED, HIGH); 
    digitalWrite(LED_GREEN, LOW); 
    digitalWrite(LED_BLUE, LOW); 
  }
  else if (choice == 2) {
    digitalWrite(LED_RED, LOW); 
    digitalWrite(LED_GREEN, HIGH); 
    digitalWrite(LED_BLUE, LOW); 
  }
  else if (choice == 3) {
    digitalWrite(LED_RED, LOW); 
    digitalWrite(LED_GREEN, LOW); 
    digitalWrite(LED_BLUE, HIGH); 
  }

} 

void print_wifi_ready_mes() {
  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");
}


// Config ESP32-CAM parameters
void init_config(camera_config_t *config) {
  config->ledc_channel = LEDC_CHANNEL_0;
  config->ledc_timer = LEDC_TIMER_0;
  config->pin_d0 = Y2_GPIO_NUM;
  config->pin_d1 = Y3_GPIO_NUM;
  config->pin_d2 = Y4_GPIO_NUM;
  config->pin_d3 = Y5_GPIO_NUM;
  config->pin_d4 = Y6_GPIO_NUM;
  config->pin_d5 = Y7_GPIO_NUM;
  config->pin_d6 = Y8_GPIO_NUM;
  config->pin_d7 = Y9_GPIO_NUM;
  config->pin_xclk = XCLK_GPIO_NUM;
  config->pin_pclk = PCLK_GPIO_NUM;
  config->pin_vsync = VSYNC_GPIO_NUM;
  config->pin_href = HREF_GPIO_NUM;
  config->pin_sscb_sda = SIOD_GPIO_NUM;
  config->pin_sscb_scl = SIOC_GPIO_NUM;
  config->pin_pwdn = PWDN_GPIO_NUM;
  config->pin_reset = RESET_GPIO_NUM;
  config->xclk_freq_hz = 20000000;
  config->frame_size = FRAMESIZE_UXGA;
  config->pixel_format = PIXFORMAT_JPEG;  // for streaming
  //config.pixel_format = PIXFORMAT_RGB565; // for face detection/recognition
  config->grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config->fb_location = CAMERA_FB_IN_PSRAM;
  config->jpeg_quality = 12;
  config->fb_count = 1;

  // if PSRAM IC present, init with UXGA resolution and higher JPEG quality
  //                      for larger pre-allocated frame buffer.
  if (config->pixel_format == PIXFORMAT_JPEG) {
    if (psramFound()) {
      config->jpeg_quality = 10;
      config->fb_count = 2;
      config->grab_mode = CAMERA_GRAB_LATEST;
    } else {   
      // Limit the frame size when PSRAM is not available
      config->frame_size = FRAMESIZE_SVGA;
      config->fb_location = CAMERA_FB_IN_DRAM;
    }
  } else {
    // Best option for face detection/recognition
    config->frame_size = FRAMESIZE_240X240;
#if CONFIG_IDF_TARGET_ESP32S3
    config->fb_count = 2;
#endif
  }
}

void init_camera() {
  // initialize camera config 
  camera_config_t config;
  init_config(&config); 

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t* s = esp_camera_sensor_get();
  
  // drop down frame size for higher initial frame rate
  if (config.pixel_format == PIXFORMAT_JPEG) {
    s->set_framesize(s, FRAMESIZE_QVGA);
  }
}

void decide_state(int *state) {
  if (Serial.available()) {
        String inputString = Serial.readStringUntil('\n');
        Serial.print("[INFO] This is from cmd_stream_handler: "); 
        Serial.println(inputString); 
        if (inputString == "stop" ) {
          Serial.println("[INFO] Valid command 'stop'."); 
          *state = STOP_STATE; 
        }
        else if (inputString == "start"){
          Serial.println("[INFO] Valid command 'start'."); 
          *state = START_STATE; 
        }
        else if (inputString == "take" ) {
          Serial.println("[INFO] Valid command 'take'."); 
          *state = TAKE_STATE; 
        }
        else if (inputString == "exit") {
          Serial.println("[INFO] Valid command 'exit'.");
          *state = EXIT_STATE;  
        }
        else {
          Serial.printf("[WARNING] Invalid command '%s'.\n", inputString); 
        }
      }
}

void turnoff_wifi_ble_cmd_switch() { wifi_ble_switch_cmd_enable = 0; }
void turnon_wifi_ble_cmd_switch() { 
  wifi_ble_switch_cmd_enable = 1; 
  httpd_exit_flag = 1; 
}

void setStartState() { state = START_STATE; }
void setTakeState() { state = TAKE_STATE; }
void setWaitState() { state = WAIT_STATE; }
void setStopState() { state = STOP_STATE; }
void setExitState() { state = EXIT_STATE; }

bool isStartState() { if (state == START_STATE) return true; else return false; }
bool isTakeState() { if (state == TAKE_STATE) return true; else return false; }
bool isWaitState() { if (state == WAIT_STATE) return true; else return false; }
bool isStopState() { if (state == STOP_STATE) return true; else return false; } 
bool isExitState() { if (state == EXIT_STATE) return true; else return false; }

bool isNowTimeup(int64_t time) {
  if (esp_timer_get_time() - time >= TWELVE_SEC_IN_MICRO) return true; 
  else return false; 
}