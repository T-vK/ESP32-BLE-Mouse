#ifndef ESP32_BLE_MOUSE_H
#define ESP32_BLE_MOUSE_H
#include "sdkconfig.h"


#if defined(CONFIG_BT_ENABLED)

// #include "BleConnectionStatus.h"
// #include "BLEHIDDevice.h"
// #include "BLECharacteristic.h"

#include "NimBLECharacteristic.h"
#include "NimBLEHIDDevice.h"

#define BLEDevice                  NimBLEDevice
#define BLEServerCallbacks         NimBLEServerCallbacks
#define BLECharacteristicCallbacks NimBLECharacteristicCallbacks
#define BLEHIDDevice               NimBLEHIDDevice
#define BLECharacteristic          NimBLECharacteristic
#define BLEAdvertising             NimBLEAdvertising
#define BLEServer                  NimBLEServer

#define MOUSE_LEFT 1
#define MOUSE_RIGHT 2
#define MOUSE_MIDDLE 4
#define MOUSE_BACK 8
#define MOUSE_FORWARD 16
#define MOUSE_ALL (MOUSE_LEFT | MOUSE_RIGHT | MOUSE_MIDDLE) # For compatibility with the Mouse library

class BleMouse : public BLEServerCallbacks, public BLECharacteristicCallbacks
{
private:
  uint8_t _buttons;
  // BleConnectionStatus* connectionStatus;
  BLEHIDDevice* hid;
  BLECharacteristic* inputMouse;
  // BLECharacteristic* outputMouse;
  BLEAdvertising*    advertising;

  uint8_t            batteryLevel;
  bool               connected = false;
  uint32_t           _delay_ms = 7;
  void buttons(uint8_t b);
  void rawAction(uint8_t msg[], char msgSize);
  void delay_ms(uint64_t ms);

  uint16_t vid       = 0x05ac;
  uint16_t pid       = 0x820a;
  uint16_t version   = 0x0210;

  // static void taskServer(void* pvParameter);
public:
  BleMouse(std::string deviceName = "ESP32 BLEMouse", std::string deviceManufacturer = "Espressif", uint8_t batteryLevel = 100);
  void begin(void);
  void end(void);
  void click(uint8_t b = MOUSE_LEFT);
  void move(signed char x, signed char y, signed char wheel = 0, signed char hWheel = 0);
  void press(uint8_t b = MOUSE_LEFT);   // press LEFT by default
  void release(uint8_t b = MOUSE_LEFT); // release LEFT by default
  bool isPressed(uint8_t b = MOUSE_LEFT); // check LEFT by default
  bool isConnected(void);
  void setBatteryLevel(uint8_t level);
  // uint8_t batteryLevel;
  std::string deviceManufacturer;
  std::string deviceName;
protected:
  virtual void onStarted(BLEServer *pServer) { };
  virtual void onConnect(BLEServer* pServer) override;
  virtual void onDisconnect(BLEServer* pServer) override;
  virtual void onWrite(BLECharacteristic* me) override;

};

#endif // CONFIG_BT_ENABLED
#endif // ESP32_BLE_MOUSE_H
