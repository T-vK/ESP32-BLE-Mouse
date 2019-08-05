#ifndef ESP32_BLE_MOUSE_H
#define ESP32_BLE_MOUSE_H
#include "sdkconfig.h"
#if defined(CONFIG_BT_ENABLED)

#include "BleConnectionStatus.h"
#include "BLEHIDDevice.h"
#include "BLECharacteristic.h"

class BleMouse {
public:
  BleMouse();
  void init();

  bool isConnected();
  void scrollDown(char units);
  void scrollUp(char units);
  void rawAction(uint8_t msg[], char msgSize);

private:
  BleConnectionStatus* connectionStatus;
  BLEHIDDevice* hid;
  BLECharacteristic* inputMouse;
  static void taskServer(void* pvParameter);
};

#endif // CONFIG_BT_ENABLED
#endif // ESP32_BLE_MOUSE_H
