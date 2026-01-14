#ifndef BLE_SERVICE_H
#define BLE_SERVICE_H

#include <Arduino.h>
#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <esp_bt.h>

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

class Bluetooth {
private:
  BLEServer *pServer;
  BLECharacteristic *pCharacteristic;

public:
  Bluetooth();
  void begin(const char *deviceName);
  void send(const String &data);
  bool isConnected();
};

#endif
