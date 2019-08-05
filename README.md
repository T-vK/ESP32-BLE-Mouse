# ESP32 BLE Mouse library

Bluetooth LE Mouse library for the ESP32  

## Intro

This library allows you to make the ESP32 act as a Bluetooth Mouse and control what it does. E.g. move the mouse, scroll, make a click etc.

## Features

This library is still under development and doesn't provide a lot of features yet.

 - [ ] Left click
 - [ ] Right click
 - [ ] Middle click
 - [ ] Back/Forwards click
 - [x] Move mouse pointer
 - [x] Scroll up/down
 - [ ] Scroll left/right

### Example

``` C++
#include <BleMouse.h>

BleMouse bleMouse;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE work!");
  bleMouse.begin();
}

void loop() {
  if(bleMouse.isConnected()) {
    Serial.println("Scroll Down");
    bleMouse.move(0,0,-1);
  }
  delay(1000);
}
```

### API docs
The BleMouse interface is almost identical to the Mouse Interface, so you can use documentation right here:
https://www.arduino.cc/reference/en/language/functions/usb/mouse/

Just remember that you have to use `BleMouse` instead of just `Mouse` and you need these two lines at the top of your script:
```
#include <BleMouse.h>
BleMouse bleMouse;
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
