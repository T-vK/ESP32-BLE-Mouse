#include "BleConnectionStatus.h"

BleConnectionStatus::BleConnectionStatus(BLECharacteristic* inputMouse) {
  this->inputMouse = inputMouse;
}

void BleConnectionStatus::onConnect(BLEServer* pServer) {
  this->connected = true;
  BLE2902* desc = (BLE2902*)inputMouse->getDescriptorByUUID(BLEUUID((uint16_t)0x2902));
  desc->setNotifications(true);
}

void BleConnectionStatus::onDisconnect(BLEServer* pServer) {
  this->connected = false;
  BLE2902* desc = (BLE2902*)inputMouse->getDescriptorByUUID(BLEUUID((uint16_t)0x2902));
  desc->setNotifications(false);
}