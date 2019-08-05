# ESP32 BLE Mouse library

Bluetooth LE Mouse library for the ESP32  

## Intro

This library allows you to make the ESP32 act as a Bluetooth Mouse and control what it does. E.g. move the mouse, scroll, make a click etc.

## Features

This library is still under development and doesn't provide a lot of features yet.

 - [ ] Left click
 - [ ] Right click
 - [ ] Middle click
 - [ ] Move mouse pointer
 - [x] Scroll up/down
 - [ ] Scroll left/right

### Example

``` C++
#include <BleMouse.h>

BleMouse bleMouse;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE work!");
  bleMouse.init();
}

void loop() {
  if(bleMouse.isConnected()) {
    Serial.println("Scroll Down by 1 unit");
    bleMouse.scrollDown(1);
  }
  delay(2000);
}
```

### API docs

``` c++
BleMouse(ReceiverSerial, TransmitterSerial); //create new instance of this class

BleMouse.init(); // needs to be called once in the beginning
BleMouse.scrollDown(units); // scroll down by the given amount of units
BleMouse.scrollUp(units); // scroll up by the given amount of units
BleMouse.rawAction(msg, msgSize);
```

### Credits

Credits to [chegewara](https://github.com/chegewara) as this library is based on [this piece of code](https://github.com/nkolban/esp32-snippets/issues/230#issuecomment-473135679) that he provided.

### More info

```
// http://www.keil.com/forum/15671/usb-mouse-with-scroll-wheel/
// Wheel Mouse - simplified version - 5 button, vertical and horizontal wheel
//
// Input report - 5 bytes
//
//     Byte | D7      D6      D5      D4      D3      D2      D1      D0
//    ------+---------------------------------------------------------------------
//      0   |  0       0       0    Forward  Back    Middle  Right   Left (Buttons)
//      1   |                             X
//      2   |                             Y
//      3   |                       Vertical Wheel
//      4   |                    Horizontal (Tilt) Wheel
//
// Feature report - 1 byte
//
//     Byte | D7      D6      D5      D4   |  D3      D2  |   D1      D0
//    ------+------------------------------+--------------+----------------
//      0   |  0       0       0       0   |  Horizontal  |    Vertical
//                                             (Resolution multiplier)
//
// Reference
//    Wheel.docx in "Enhanced Wheel Support in Windows Vista" on MS WHDC
//    http://www.microsoft.com/whdc/device/input/wheel.mspx
//
```
