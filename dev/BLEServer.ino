#include "Arduino.h"
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

class BLE_Server_CB: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      Serial.print(millis()/1000);
      Serial.println(" Client connected");
    }
    void onDisconnect(BLEServer* pServer) {
      Serial.print(millis()/1000);
      Serial.println(" Client disconnected");
    }
};

class BLE_Characteristic_CB: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();
      if (value.length() > 0) {
        Serial.println("*********");
        Serial.print("New value: ");
        for (int i = 0; i < value.length(); i++)
          Serial.print(value[i]);
        Serial.println();
        Serial.println("*********");
      }
    }
};

void setup()
{
  Serial.begin(115200);
  Serial.println("setup");

  BLEDevice::init("ESP32");
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new BLE_Server_CB());
  BLEService *pService = pServer->createService(SERVICE_UUID);
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  pCharacteristic->setCallbacks(new BLE_Characteristic_CB());
  pCharacteristic->setValue("Hello World");
  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();

  Serial.println("BLE device advertising");
}

void loop()
{
  delay(100);
}
