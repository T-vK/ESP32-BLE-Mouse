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

/*
static const uint8_t _hidReportDescriptor[] = {
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
};*/

//
// Wheel Mouse - simplified version
//
// Input report - 5 bytes
//
// Byte | D7 D6 D5 D4 D3 D2 D1 D0
// ------+---------------------------------------------------------------------
// 0 | 0 0 0 Forward Back Middle Right Left (Button)
// 1 | X
// 2 | Y
// 3 | Vertical Wheel
// 4 | Horizontal (Tilt) Wheel
//
// Feature report - 1 byte
//
// Byte | D7 D6 D5 D4 | D3 D2 | D1 D0
// ------+------------------------------+--------------+----------------
// 0 | 0 0 0 0 | Horizontal | Vertical
// (Resolution multiplier)
//
// Reference
// Wheel.docx in "Enhanced Wheel Support in Windows Vista" on MS WHDC
// http://www.microsoft.com/whdc/device/input/wheel.mspx
//

static const uint8_t _hidReportDescriptor[] = {
  0x05, 0x01, // USAGE_PAGE (Generic Desktop)
  0x09, 0x02, // USAGE (Mouse)
  0xa1, 0x01, // COLLECTION (Application)
  0x09, 0x02, // USAGE (Mouse)
  0xa1, 0x02, // COLLECTION (Logical)
  0x09, 0x01, // USAGE (Pointer)
  0xa1, 0x00, // COLLECTION (Physical)
  // ------------------------------ Buttons
  0x05, 0x09, // USAGE_PAGE (Button)
  0x19, 0x01, // USAGE_MINIMUM (Button 1)
  0x29, 0x05, // USAGE_MAXIMUM (Button 5)
  0x15, 0x00, // LOGICAL_MINIMUM (0)
  0x25, 0x01, // LOGICAL_MAXIMUM (1)
  0x75, 0x01, // REPORT_SIZE (1)
  0x95, 0x05, // REPORT_COUNT (5)
  0x81, 0x02, // INPUT (Data,Var,Abs)
  // ------------------------------ Padding
  0x75, 0x03, // REPORT_SIZE (3)
  0x95, 0x01, // REPORT_COUNT (1)
  0x81, 0x03, // INPUT (Cnst,Var,Abs)
  // ------------------------------ X,Y position
  0x05, 0x01, // USAGE_PAGE (Generic Desktop)
  0x09, 0x30, // USAGE (X)
  0x09, 0x31, // USAGE (Y)
  0x15, 0x81, // LOGICAL_MINIMUM (-127)
  0x25, 0x7f, // LOGICAL_MAXIMUM (127)
  0x75, 0x08, // REPORT_SIZE (8)
  0x95, 0x02, // REPORT_COUNT (2)
  0x81, 0x06, // INPUT (Data,Var,Rel)
  0xa1, 0x02, // COLLECTION (Logical)
  // ------------------------------ Vertical wheel res multiplier
  0x09, 0x48, // USAGE (Resolution Multiplier)
  0x15, 0x00, // LOGICAL_MINIMUM (0)
  0x25, 0x01, // LOGICAL_MAXIMUM (1)
  0x35, 0x01, // PHYSICAL_MINIMUM (1)
  0x45, 0x04, // PHYSICAL_MAXIMUM (4)
  0x75, 0x02, // REPORT_SIZE (2)
  0x95, 0x01, // REPORT_COUNT (1)
  0xa4, // PUSH
  0xb1, 0x02, // FEATURE (Data,Var,Abs)
  // ------------------------------ Vertical wheel
  0x09, 0x38, // USAGE (Wheel)
  0x15, 0x81, // LOGICAL_MINIMUM (-127)
  0x25, 0x7f, // LOGICAL_MAXIMUM (127)
  0x35, 0x00, // PHYSICAL_MINIMUM (0) - reset physical
  0x45, 0x00, // PHYSICAL_MAXIMUM (0)
  0x75, 0x08, // REPORT_SIZE (8)
  0x81, 0x06, // INPUT (Data,Var,Rel)
  0xc0, // END_COLLECTION
  0xa1, 0x02, // COLLECTION (Logical)
  // ------------------------------ Horizontal wheel res multiplier
  0x09, 0x48, // USAGE (Resolution Multiplier)
  0xb4, // POP
  0xb1, 0x02, // FEATURE (Data,Var,Abs)
  // ------------------------------ Padding for Feature report
  0x35, 0x00, // PHYSICAL_MINIMUM (0) - reset physical
  0x45, 0x00, // PHYSICAL_MAXIMUM (0)
  0x75, 0x04, // REPORT_SIZE (4)
  0xb1, 0x03, // FEATURE (Cnst,Var,Abs)
  // ------------------------------ Horizontal wheel
  0x05, 0x0c, // USAGE_PAGE (Consumer Devices)
  0x0a, 0x38, 0x02, // USAGE (AC Pan)
  0x15, 0x81, // LOGICAL_MINIMUM (-127)
  0x25, 0x7f, // LOGICAL_MAXIMUM (127)
  0x75, 0x08, // REPORT_SIZE (8)
  0x81, 0x06, // INPUT (Data,Var,Rel)
  0xc0, // END_COLLECTION
  0xc0, // END_COLLECTION
  0xc0, // END_COLLECTION
  0xc0 // END_COLLECTION
};


BleMouse::BleMouse(void) : _buttons(0)
{
  this->connectionStatus = new BleConnectionStatus();
}

void BleMouse::begin(void)
{
  xTaskCreate(this->taskServer, "server", 20000, (void *)this, 5, NULL);
}

void BleMouse::end(void)
{
}

void BleMouse::click(uint8_t b)
{
	_buttons = b;
	move(0,0,0,0);
	_buttons = 0;
	move(0,0,0,0);
}

void BleMouse::move(signed char x, signed char y, signed char wheel, signed char hWheel)
{
  if (this->isConnected())
  {
    uint8_t m[4];
  	m[0] = _buttons;
  	m[1] = x;
  	m[2] = wheel;
  	m[3] = hWheel;
    this->inputMouse->setValue(m, 4);
    this->inputMouse->notify();
  }
}

void BleMouse::buttons(uint8_t b)
{
	if (b != _buttons)
	{
		_buttons = b;
		move(0,0,0,0);
	}
}

void BleMouse::press(uint8_t b)
{
	buttons(_buttons | b);
}

void BleMouse::release(uint8_t b)
{
	buttons(_buttons & ~b);
}

bool BleMouse::isPressed(uint8_t b)
{
	if ((b & _buttons) > 0)
		return true;
	return false;
}

bool BleMouse::isConnected(void) {
  return this->connectionStatus->connected;
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

  bleMouseInstance->hid->reportMap((uint8_t*)_hidReportDescriptor, sizeof(_hidReportDescriptor));
  bleMouseInstance->hid->startServices();

  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->setAppearance(HID_MOUSE);
  pAdvertising->addServiceUUID(bleMouseInstance->hid->hidService()->getUUID());
  pAdvertising->start();
  bleMouseInstance->hid->setBatteryLevel(7);

  ESP_LOGD(LOG_TAG, "Advertising started!");
  vTaskDelay(portMAX_DELAY); //delay(portMAX_DELAY);
}
