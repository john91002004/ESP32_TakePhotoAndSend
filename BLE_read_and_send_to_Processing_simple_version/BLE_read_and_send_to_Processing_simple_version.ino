/**
 * A BLE client example that is rich in capabilities.
 * There is a lot new capabilities implemented.
 * author unknown
 * updated by chegewara
 */

#include "BLEDevice.h"
//#include "BLEScan.h"
#include <string.h> 
#include <SPIFFS.h>
#include "esp_timer.h"

#define FILE_PHOTO "/photo_rx.jpg"
#define DATABUF_LEN 500

// Local hostname (according to your choice above)
#define CLIENT_02  // this can be 01/02/03

#ifdef CLIENT_01 
static std::string ESP_HOSTNAME = "ESP_client_01";
static std::string C_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8"; 
#endif
#ifdef CLIENT_02 
static std::string ESP_HOSTNAME = "ESP_client_02";
static std::string C_UUID = "5a95d12e-fa74-48d7-b473-e644e750e7be"; 
#endif
#ifdef CLIENT_03 
static std::string ESP_HOSTNAME = "ESP_client_03";
static std::string C_UUID = "6a8ab204-228d-44d4-9162-2e743e3a02b3"; 
#endif

// Remote server name 
static std::string remoteServerName = "ESP32_notify_server"; 
// The remote service we wish to connect to.
static BLEUUID serviceUUID("4fafc201-1fb5-459e-8fcc-c5c9c331914b");
// The characteristic of the remote service we are interested in.
static BLEUUID    charUUID(C_UUID);

static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;
static BLERemoteCharacteristic* pRemoteCharacteristic;
static BLEAdvertisedDevice* myDevice;

int file_open_mode = 1;   // 0 --> read, 1 --> write, 2 --> append
int count = 0; 
int64_t reboot_time; 

static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {
    /*Serial.print("Notify callback for characteristic ");
    Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
    Serial.print(" of data length ");
    Serial.print(length);
//    Serial.print(" and write data: "); 
//    Serial.write(pData, length);
    Serial.print(", and print data: "); 
    for (int i=0;i<length;i++) {
      Serial.print(*(pData+i)); 
      Serial.print(" "); 
    }
    Serial.println(); */

    // open file to save 
    File file; 
    if (file_open_mode == 1) {
      log_i("SPIFFS open file: %s in FILE_WRITE mode.", FILE_PHOTO); 
      file = SPIFFS.open(FILE_PHOTO, FILE_WRITE); 
      file_open_mode = 2;
    }
    else if (file_open_mode == 2) {
      log_i("SPIFFS open file: %s in FILE_APPEND mode.", FILE_PHOTO);
      file = SPIFFS.open(FILE_PHOTO, FILE_APPEND); 
    }
    file.write(pData, length); 
    file.close(); 
    log_i("Done written."); 
}

class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {
    //Serial.println("onConnect: connection succeeded."); 
  }

  void onDisconnect(BLEClient* pclient) {
    connected = false;
    //Serial.println("onDisconnect: connection ended.");
  }
};

bool connectToServer() {
    Serial.print("Forming a connection to ");
    Serial.println(myDevice->getAddress().toString().c_str());
    
    BLEClient*  pClient  = BLEDevice::createClient();
    Serial.println(" - Created client");

    pClient->setClientCallbacks(new MyClientCallback());

    // Connect to the remove BLE Server.
    pClient->connect(myDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
    Serial.println(" - Connected to server");
    pClient->setMTU(517); //set client to request maximum MTU from server (default is 23 otherwise)
  
    // Obtain a reference to the service we are after in the remote BLE server.
    BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
    if (pRemoteService == nullptr) {
      Serial.print("Failed to find our service UUID: ");
      Serial.println(serviceUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    Serial.println(" - Found our service");


    // Obtain a reference to the characteristic in the service of the remote BLE server.
    pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
    if (pRemoteCharacteristic == nullptr) {
      Serial.print("Failed to find our characteristic UUID: ");
      Serial.println(charUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    Serial.println(" - Found our characteristic");

    // Read the value of the characteristic.
    if(pRemoteCharacteristic->canRead()) {
      std::string value = pRemoteCharacteristic->readValue();
      Serial.print("The characteristic value was: ");
      Serial.println(value.c_str());
    }

    if(pRemoteCharacteristic->canNotify())
      pRemoteCharacteristic->registerForNotify(notifyCallback);

    connected = true;

    //std::string hello_mes = "Hi, this is " + ESP_HOSTNAME; 
    //pRemoteCharacteristic->writeValue(hello_mes.c_str(), hello_mes.length());
    return true;
}
/**
 * Scan for BLE servers and find the first one that advertises the service we are looking for.
 */
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
 /**
   * Called for each advertising BLE server.
   */
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    Serial.print("BLE Advertised Device: ");
    Serial.println(advertisedDevice.toString().c_str());
    if (advertisedDevice.haveName()) {
      if (advertisedDevice.getName() == remoteServerName) {
        Serial.print("We found the remote server: "); 
        Serial.println(remoteServerName.c_str());  
        Serial.println("Stop scanning for new devices."); 
        BLEDevice::getScan()->stop();   // this is a singleton, so there's only one instance.

        // get the remote server
        myDevice = new BLEAdvertisedDevice(advertisedDevice);
        doConnect = true;   // an indicator telling loop() to connect 
        doScan = true;  
      }

    } // Found our server
  } // onResult
}; // MyAdvertisedDeviceCallbacks


void setup() {
  reboot_time = esp_timer_get_time(); 
  Serial.begin(115200);

  // mount SPIFFS
  if(!SPIFFS.begin(true)){
    log_e("can not mount SPIFFS!");
    ESP.restart(); 
  }
  else {
    delay(50);
    log_i("SPIFFS mounted."); 
  }

  Serial.println("Starting Arduino BLE Client application...");
  BLEDevice::init(ESP_HOSTNAME);

  // Retrieve a Scanner and set the callback we want to use to be informed when we
  // have detected a new device.  Specify that we want active scanning and start the
  // scan to run for 5 seconds.
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(0);
} // End of setup.


// This is the Arduino main loop function.
void loop() {

  // 15 seconds after will set file open mode as write to clear previous image
  if (file_open_mode == 2) {
    if (++count == 15) {   
      count = 0; 
      file_open_mode = 1; 
      File file = SPIFFS.open(FILE_PHOTO);
      int size = file.size(); 
      log_i("Last file received has total size: %d", size); 
      log_i("Reset file write mode to FILE_WRITE."); 
      
      log_i("Send file to Processing in PC."); 

      // Send header and data and ender
      char dataBuf[DATABUF_LEN] = ""; 
      String header_txt = "$HEADER_TXT$"; 
      String ender_txt = "$ENDER_TXT$"; 

      header_txt.toCharArray(dataBuf, header_txt.length()+1); 
      Serial.write(dataBuf, header_txt.length());

      int remaining_len = size; 
      int len_send; 
      while (remaining_len != 0) {
        delay(10); 
        len_send = min(remaining_len, DATABUF_LEN); 
        file.readBytes(dataBuf, len_send); 
        Serial.write(dataBuf, len_send);

        remaining_len -= len_send;
      }

      ender_txt.toCharArray(dataBuf, ender_txt.length()+1); 
      Serial.write(dataBuf, ender_txt.length()); 

      file.close(); 
      log_i("Done sending."); 
      ESP.restart(); 

    } 
  }

  // If the flag "doConnect" is true then we have scanned for and found the desired
  // BLE Server with which we wish to connect.  Now we connect to it.  Once we are 
  // connected we set the connected flag to be true.
  if (doConnect == true) {
    if (connectToServer()) {
      Serial.println("We are now connected to the BLE Server.");
    } else {
      Serial.println("We have failed to connect to the server; there is nothin more we will do.");
    }
    doConnect = false;
  }

  // If we are connected to a peer BLE Server, update the characteristic each time we are reached
  // with the current time since boot.
  if (connected) {/*
    String newValue = "Time since boot: " + String(millis()/1000);
    Serial.println("Setting new characteristic value to \"" + newValue + "\"");
    
    // Set the characteristic's value to be the array of bytes that is actually a string.
    pRemoteCharacteristic->writeValue(newValue.c_str(), newValue.length());*/
  }else if(doScan){
    BLEDevice::getScan()->start(0);  // this is just example to start scan after disconnect, most likely there is better way to do it in arduino
  }
  
  delay(1000); // Delay a second between loops.
} // End of loop
