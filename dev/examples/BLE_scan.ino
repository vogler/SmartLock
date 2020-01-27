/*
   Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleScan.cpp
   Ported to Arduino ESP32 by Evandro Copercini
*/

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

int scanTime = 5; //In seconds
BLEScan* pBLEScan;

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      Serial.printf("Advertised Device: %s \n", advertisedDevice.toString().c_str());
    }
};

// only when advertising via nRF Connect:
// Advertised Device: Name: Pixel 2 XL, Address: ..., manufacturer data: aaaaaaaa

// only if not connected to phone:
// Advertised Device: Name: Forerunner 935, Address: ..., manufacturer data: 87000a83, serviceUUID: 6a4e3e10-667b-11e3-949a-0800200c9a66 

// only if on but also if already connected to phone:
// Advertised Device: Name: LE-Bose QuietComfort 35, Address: ..., manufacturer data: 010171127f6315a02224258db706fc, serviceUUID: 0000febe-0000-1000-8000-00805f9b34fb, txPower: 4

// always? TODO check when connected to watch/after run:
// Advertised Device: Name: Stryd, Address: ..., manufacturer data: aaaa2a5c000258c3, serviceUUID: 00001818-0000-1000-8000-00805f9b34fb 

// (black) Advertised Device: Name: iTAG            , Address: ...:a4, appearance: 961, serviceUUID: 0000ffe0-0000-1000-8000-00805f9b34fb, txPower: 0 
// (blue)  Advertised Device: Name: iTAG            , Address: ...:6d, appearance: 961, serviceUUID: 0000ffe0-0000-1000-8000-00805f9b34fb, txPower: 0 

// unknown stuff (nRF Connect: A = Apple, W = Windows):
// ? Advertised Device: Name: , Address: 28:56:5a:5a:45:38, manufacturer data: 2d0102000110e211b4f192a9423faa6817f654b3e12a7a3cbce4e4b6 
// ? Advertised Device: Name: , Address: 44:bf:6f:e7:00:15, manufacturer data: 4c001005411c3084d5, txPower: 12 
// A Advertised Device: Name: , Address: 47:27:ac:c5:83:08, manufacturer data: 4c001006231ac1e39887, txPower: 7 
// A Advertised Device: Name: , Address: 54:07:97:7d:93:31, manufacturer data: 4c001006131ad607d8d3, txPower: 12 
// A Advertised Device: Name: , Address: 55:a4:21:0c:24:4c, manufacturer data: 4c001005411c9baed3, txPower: 12 
// W Advertised Device: Name: , Address: 6c:f3:31:aa:f2:12, manufacturer data: 060001092002db0ae4d0dfe2abf38cf21b810de65b8c1abfd4419733f8 
// ? Advertised Device: Name: , Address: 7b:6d:9e:80:66:e9, manufacturer data: 060001092002e6ac9f39fc46cb0da1c10f2f8c9a58356c435b246fd036 
// ? Advertised Device: Name: , Address: c0:48:e6:1f:8a:34, manufacturer data: 75004204018060c048e61f8a34c248e61f8a3301000000000000
// ? Advertised Device: Name: , Address: c0:97:27:19:f8:7f, manufacturer data: 75004204018060c0972719f87fc2972719f87e01330000000000 

// not mine:
// Advertised Device: Name: LE-Bose Revolve SoundLink, Address: 4c:87:5d:13:5c:05, manufacturer data: 100340100230, serviceUUID: 0000febe-0000-1000-8000-00805f9b34fb, txPower: 10 
// Advertised Device: Name: LE_SRS-XB21, Address: c7:df:15:15:b5:ae, manufacturer data: 2d010400010108019fd513e1000ec1000000000000 


void setup() {
  Serial.begin(115200);
  Serial.println("Scanning...");

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);  // less or equal setInterval value
}

void loop() {
  // put your main code here, to run repeatedly:
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  Serial.print("Devices found: ");
  Serial.println(foundDevices.getCount());
  Serial.println("Scan done!\n\n");
  pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory
  delay(2000);
}