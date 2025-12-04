#include "ble_service.h"

static bool _deviceConnected = false;

class ServerCallbacks : public BLEServerCallbacks
{
  void onConnect(BLEServer *pServer)
  {
    _deviceConnected = true;
    Serial.println("BLE Client connected");
  }

  void onDisconnect(BLEServer *pServer)
  {
    _deviceConnected = false;
    Serial.println("BLE Client disconnected");
    pServer->startAdvertising();
  }
};

Bluetooth::Bluetooth()
    : pServer(nullptr), pCharacteristic(nullptr)
{
}

void Bluetooth::begin(const char *deviceName)
{
  BLEDevice::init(deviceName);

  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new ServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);

  pCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_WRITE |
          BLECharacteristic::PROPERTY_NOTIFY |
          BLECharacteristic::PROPERTY_INDICATE);

  pCharacteristic->addDescriptor(new BLE2902());
  pService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);
  BLEDevice::startAdvertising();

  Serial.println("BLE started, waiting for connections...");
}

void Bluetooth::send(const String &data)
{
  if (_deviceConnected)
  {
    pCharacteristic->setValue(data.c_str());
    pCharacteristic->notify();

    Serial.print("BLE sent: ");
    Serial.println(data);
  }
}

bool Bluetooth::isConnected()
{
  return _deviceConnected;
}
