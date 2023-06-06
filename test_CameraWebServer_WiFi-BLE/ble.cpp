#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// ===========================
// Ble variables
// ===========================
BLEServer* pServer = NULL;
BLECharacteristic* p1 = NULL, *p2 = NULL, *p3 = NULL;
int deviceConnected = 0;
bool oldDeviceConnected = false;
uint32_t value = 0;
char* s[] = {"This is p1: ", "This is p2: ", "URLGREAT", "ROCF", "ISO27001 is good!"} ; 

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

void BleServer_init() {

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  Serial.println("[FLAG] BleServer_init 3"); 
  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  Serial.println("[FLAG] BleServer_init 4"); 
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

  Serial.println("[FLAG] BleServer_init 5"); 
  // Create a BLE Descriptor
  p1->addDescriptor(new BLE2902());
  p2->addDescriptor(new BLE2902());
  p3->addDescriptor(new BLE2902());

  Serial.println("[FLAG] BleServer_init 6"); 
  // Start the service
  pService->start();

  Serial.println("[FLAG] BleServer_init 7"); 
  // Start advertising
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