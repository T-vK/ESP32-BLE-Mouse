# ESP32 BLE Mouse library

This library allows you to make the ESP32 act as a Bluetooth Mouse and control what it does. E.g. move the mouse, scroll, make a click etc.

You might also be interested in:
- [ESP32-BLE-Keyboard](https://github.com/T-vK/ESP32-BLE-Keyboard)
- [ESP32-BLE-Gamepad](https://github.com/lemmingDev/ESP32-BLE-Gamepad)

## Features

 - [x] Left click
 - [x] Right click
 - [x] Middle click
 - [x] Back/Forwards click
 - [x] Move mouse pointer left/right
 - [x] Move mouse pointer up/down
 - [x] Scroll up/down
 - [x] Scroll left/right
 - [x] Report optional battery level to host (basically works, but it doesn't show up in Android's status bar)
 - [x] Customize Bluetooth device name/manufacturer
 - [x] Compatible with Android
 - [x] Compatible with Windows
 - [x] Compatible with Linux
 - [x] Compatible with MacOS X (not stable, some people have issues, doesn't work with old devices)
 - [x] Compatible with iOS (not stable, some people have issues, doesn't work with old devices)
 
## Installation
- (Make sure you can use the ESP32 with the Arduino IDE. [Instructions can be found here.](https://github.com/espressif/arduino-esp32#installation-instructions))
- [Download the latest release of this library from the release page.](https://github.com/T-vK/ESP32-BLE-Mouse/releases)
- In the Arduino IDE go to "Sketch" -> "Include Library" -> "Add .ZIP Library..." and select the file you just downloaded.
- You can now go to "File" -> "Examples" -> "ESP32 BLE Mouse" and select any of the examples to get started.

## Example

``` C++
/**
 * This example turns the ESP32 into a Bluetooth LE mouse that scrolls down every 2 seconds.
 */
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
  delay(2000);
}
```

## API docs
The BleMouse interface is almost identical to the Mouse Interface, so you can use documentation right here:
https://www.arduino.cc/reference/en/language/functions/usb/mouse/

Just remember that you have to use `bleMouse` instead of just `Mouse` and you need these two lines at the top of your script:
```
#include <BleMouse.h>
BleMouse bleMouse;
```

This library supports a few additional features that the Mouse library does not support at the time of writing:

- Scrolling left/right E.g.: `bleMouse.move(0,0,0,1)` (Scroll left) and `bleMouse.move(0,0,0,-1)` (Scroll right)
- Using the back and forward buttons E.g.: `bleMouse.click(MOUSE_BACK)` and `bleMouse.click(MOUSE_FORWARD)`

There is also Bluetooth specific information that you can use (optional):

Instead of `BleMouse bleMouse;` you can do `BleMouse bleMouse("Bluetooth Device Name", "Bluetooth Device Manufacturer", 100);`.
The third parameter is the initial battery level of your device. To adjust the battery level later on you can simply call e.g.  `bleMouse.setBatteryLevel(50)` (set battery level to 50%).
By default the battery level will be set to 100%, the device name will be `ESP32 Bluetooth Mouse` and the manufacturer will be `Espressif`.


## Credits

Credits to [chegewara](https://github.com/chegewara) as this library is based on [this piece of code](https://github.com/nkolban/esp32-snippets/issues/230#issuecomment-473135679) that he provided.
