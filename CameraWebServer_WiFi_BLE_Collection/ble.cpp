#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <esp_bt.h>
#include <esp_bt_main.h>
#include <esp_err.h> 
#include <SPIFFS.h>

/*-----*/
#define FILE_PHOTO "/photo.jpg"
#define PACKET_LEN 500
// ===========================
// Ble variables
// ===========================
BLEServer* pServer = NULL;
BLECharacteristic* p1 = NULL, *p2 = NULL, *p3 = NULL;
BLEService *pService = NULL;
int deviceConnected = 0;
uint32_t value = 0;
char buf[PACKET_LEN]; 
int remaining_len, tx_len; 

extern int ble_state; 

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define C1_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define C2_UUID "5a95d12e-fa74-48d7-b473-e644e750e7be"
#define C3_UUID "6a8ab204-228d-44d4-9162-2e743e3a02b3"

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected++;
      Serial.print("We get one more connected. Totol connection: ");
      Serial.println(deviceConnected); 
      BLEDevice::startAdvertising(); 
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected--;
      Serial.print("We get one less connected. Totol connection: ");
      Serial.println(deviceConnected); 
      pServer->startAdvertising();  
      Serial.println("restart advertising");
    }
};

void stop_ble() {
  BLEDevice::stopAdvertising(); 
  BLEDevice::deinit(false); 
  ble_state = 0; 
}

void start_ble() {
  ble_state = 1; 

  // Create the BLE Server
  // BLEDevice::init() does: btStart(), esp_bt_controller_init(), esp_bt_controller_enable(), esp_bluedroid_init(), esp_bluedroid_enable(), 
  // esp_ble_gap_register_callback(), esp_ble_gattc_register_callback(), esp_ble_gatts_register_callback(), esp_ble_gap_set_device_name() 
  BLEDevice::init("ESP32_notify_server");   

  // BLEDevice::m_pServer is singleton
  // createServer() does: BLEDevice::m_pServer = new BLEServer(), ->createApp() 
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  // and bind this service to the server when do BLEService::executeCreate()
  pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  p1 = pService->createCharacteristic(
                      C1_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );
  
  p2 = pService->createCharacteristic(
                      C2_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );

  p3 = pService->createCharacteristic(
                      C3_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );

  // Create a BLE Descriptor
  p1->addDescriptor(new BLE2902());
  p2->addDescriptor(new BLE2902());
  p3->addDescriptor(new BLE2902());

  // Start the service
  pService->start();

  // Start advertising
  // new a BLEAdvertising object if none (singleton)
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
  BLEDevice::startAdvertising();
  Serial.println("Waiting a client connection to notify...");
}

void receiveAndNotify(int site_choice) {
  File file; 
  file = SPIFFS.open(FILE_PHOTO); 
  int remaining_len = (int)file.size();

  log_d("[INFO] Photo file size: %d\n", remaining_len); 

  if (deviceConnected) {
    log_d("[FLAG] in device connected, site_choice = %d", site_choice); 

    delay(3000);  // let remote client have time to breathe 

    BLECharacteristic* pn; 
    if (site_choice == 1) {
      pn = p1; 
    } 
    else if (site_choice == 2) {
      pn = p2; 
    }
    else if (site_choice == 3) {
      pn = p3; 
    } 
    else {
      log_w("[WARN] No such site.");
    }

    while (remaining_len > 0 ) {
      log_d("[FLAG] remaining_len = %d", remaining_len);

      tx_len = std::min(remaining_len, PACKET_LEN); 
      file.readBytes(buf, tx_len); 
      pn->setValue((uint8_t *)buf, tx_len);
      pn->notify(); 
      remaining_len = remaining_len - tx_len; 
      delay(10); 
    }

  }
  
  file.close(); 
  log_d("[FLAG] file closed"); 
}

int getDeviceConnected() {
  return deviceConnected; 
}