#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include "BLE2902.h"
#include "BLEHIDDevice.h"
#include "HIDTypes.h"
#include "HIDKeyboardTypes.h"
#include <driver/adc.h>
#include "sdkconfig.h"

#include "BleConnectionStatus.h"
#include "BleMouse.h"

#if defined(CONFIG_ARDUHAL_ESP_LOG)
#include "esp32-hal-log.h"
#define LOG_TAG ""
#else
#include "esp_log.h"
static const char* LOG_TAG = "BLEDevice";
#endif

BleMouse::BleMouse() {
  this->connectionStatus = new BleConnectionStatus();
}

void BleMouse::init() {
  xTaskCreate(this->taskServer, "server", 20000, (void *)this, 5, NULL);
}

void BleMouse::taskServer(void* pvParameter) {
  BleMouse* bleMouseInstance = (BleMouse *) pvParameter; //static_cast<BleMouse *>(pvParameter);
  BLEDevice::init("ESP32-BLE-Mouse");
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(bleMouseInstance->connectionStatus);

  bleMouseInstance->hid = new BLEHIDDevice(pServer);
  bleMouseInstance->inputMouse = bleMouseInstance->hid->inputReport(1); // <-- input REPORTID from report map
  bleMouseInstance->connectionStatus->inputMouse = bleMouseInstance->inputMouse;

  std::string name = "chegewara";
  bleMouseInstance->hid->manufacturer()->setValue(name);

  bleMouseInstance->hid->pnp(0x02, 0xe502, 0xa111, 0x0210);
  bleMouseInstance->hid->hidInfo(0x00,0x02);

  BLESecurity *pSecurity = new BLESecurity();

  pSecurity->setAuthenticationMode(ESP_LE_AUTH_BOND);

  const uint8_t reportMapMouse[] = {
    USAGE_PAGE(1),       0x01,
    USAGE(1),            0x02,
    COLLECTION(1),       0x01,
    REPORT_ID(1),        0x01,
    USAGE(1),            0x01,
    COLLECTION(1),       0x00,
    USAGE_PAGE(1),       0x09,
    USAGE_MINIMUM(1),    0x1,
    USAGE_MAXIMUM(1),    0x3,
    LOGICAL_MINIMUM(1),  0x0,
    LOGICAL_MAXIMUM(1),  0x1,
    REPORT_COUNT(1),     0x3,
    REPORT_SIZE(1),      0x1,
    0x80|0x01,           0x2,    // (Data, Variable, Absolute), ;3 button bits
    REPORT_COUNT(1),     0x1,
    REPORT_SIZE(1),      0x5,
    0x80|0x01,           0x1,    //(Constant), ;5 bit padding
    USAGE_PAGE(1),       0x1,    //(Generic Desktop),
    USAGE(1),            0x30,
    USAGE(1),            0x31,
    USAGE(1),            0x38,
    LOGICAL_MINIMUM(1),  0x81,
    LOGICAL_MAXIMUM(1),  0x7f,
    REPORT_SIZE(1),      0x8,
    REPORT_COUNT(1),     0x3,
    0x80|0x01,           0x6,    //(Data, Variable, Relative), ;2 position bytes (X & Y)
    END_COLLECTION(0),
    END_COLLECTION(0)
  };

  bleMouseInstance->hid->reportMap((uint8_t*)reportMapMouse, sizeof(reportMapMouse));
  bleMouseInstance->hid->startServices();

  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->setAppearance(HID_MOUSE);
  pAdvertising->addServiceUUID(bleMouseInstance->hid->hidService()->getUUID());
  pAdvertising->start();
  bleMouseInstance->hid->setBatteryLevel(7);

  ESP_LOGD(LOG_TAG, "Advertising started!");
  vTaskDelay(portMAX_DELAY); //delay(portMAX_DELAY);
}

bool BleMouse::isConnected() {
  return this->connectionStatus->connected;
}

void BleMouse::rawAction(uint8_t msg[], char msgSize) {
  this->inputMouse->setValue(msg, msgSize);
  this->inputMouse->notify();
}

void BleMouse::scrollDown(char amount) {
  if(this->isConnected()) {
    uint8_t msg[] = { 0x00, 0x00, 0x00, amount };
    this->rawAction(msg, 4);
  }
}

void BleMouse::scrollDown() {
  this->scrollDown(1);
}

void BleMouse::scrollUp(char amount) {
  if(this->isConnected()) {
    uint8_t msg[] = { 0x00, 0x00, 0x00, amount==0 ? 0x0 : amount+0x80 };
    this->rawAction(msg, 4);
  }
}

void BleMouse::scrollUp() {
  this->scrollUp(1);
}
