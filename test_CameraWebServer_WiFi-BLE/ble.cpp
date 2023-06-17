#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <esp_bt.h>
#include <esp_bt_main.h>
#include <esp_err.h> 

// ===========================
// Ble variables
// ===========================
BLEServer* pServer = NULL;
BLECharacteristic* p1 = NULL, *p2 = NULL, *p3 = NULL;
BLEService *pService = NULL;
int deviceConnected = 0;
bool oldDeviceConnected = false;
uint32_t value = 0;
char* s[] = {"This is p1: ", "This is p2: ", "URLGREAT", "ROCF", "ISO27001 is good!"} ; 

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

void stop_ble_deinit_disable() {
  BLEDevice::stopAdvertising(); 
  esp_bluedroid_disable();
  esp_bluedroid_deinit();
  esp_bt_controller_disable();
  esp_bt_controller_deinit();
  ble_state = 0; 
}

void restart_ble_init_enable() {
  esp_err_t errRc = ESP_OK; 
  if (!btStart()) {
    Serial.println("[ERROR] Fail to btStart()."); 
    return ;
  }
  
  errRc = esp_bluedroid_init();
  if (errRc != ESP_OK) {
    Serial.println("[ERROR] Fail to esp_bluedroid_init()."); 
    return ;
  }
  errRc = esp_bluedroid_enable(); 
  if (errRc != ESP_OK) {
    Serial.println("[ERROR] Fail to esp_bluedroid_enable()."); 
    return ; 
  }
  
  BLEDevice::startAdvertising(); 
  ble_state = 1; 
}

void stop_ble() {
  BLEDevice::stopAdvertising(); 
  BLEDevice::deinit(false); 
  ble_state = 0; 
}

void restart_ble() {
  BLEDevice::init("ESP32_notify_server"); 
  BLEDevice::startAdvertising(); 
  ble_state = 1; 
}

void start_ad() {
  BLEDevice::startAdvertising();
}

void stop_ad() {
  BLEDevice::stopAdvertising();
}

void clearall_ble() {
  BLEDevice::stopAdvertising(); 
  BLEDevice::deinit(true); 
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

void receiveAndNotify() {
  if (deviceConnected) {
    if (p1->getLength() != 4 ) {    // the value of p1->setValue() will be set to p1->getValue() which is weird, so kill it
      Serial.print("p1 got message with length ");
      Serial.print(p1->getLength());
      Serial.print(": "); 
      Serial.println(p1->getValue().c_str()); 
    }
    if (p2->getLength() != 4 ) {    // the value of p2->setValue() will be set to p1->getValue() which is weird, so kill it
      Serial.print("p2 got message with length ");
      Serial.print(p2->getLength());
      Serial.print(": "); 
      Serial.println(p2->getValue().c_str()); 
    }
    if (p3->getLength() != 4 ) {    // the value of p3->setValue() will be set to p1->getValue() which is weird, so kill it
      Serial.print("p3 got message with length ");
      Serial.print(p3->getLength());
      Serial.print(": "); 
      Serial.println(p3->getValue().c_str()); 
    }

    value = value + 31; 
    p1->setValue((uint8_t*) &value, 4); 
    p1->notify();
    Serial.print("p1 notify ");
    Serial.println(value); 
    
    value = value + 31; 
    p2->setValue((uint8_t*) &value, 4); 
    p2->notify();
    Serial.print("p2 notify ");
    Serial.println(value); 

    value = value + 31; 
    p3->setValue((uint8_t*) &value, 4); 
    p3->notify();
    Serial.print("p3 notify ");
    Serial.println(value); 

    delay(5000); 
  }

  // disconnecting
  if (!deviceConnected && oldDeviceConnected) {
      delay(500); // give the bluetooth stack the chance to get things ready
      oldDeviceConnected = false;
  }
  // connecting
  if (deviceConnected && !oldDeviceConnected) {
      // do stuff here on connecting
      oldDeviceConnected = true;
  }
}