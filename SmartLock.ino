// 3D printed smart lock using:
// - bare ESP32 (WROOM-32)
//   - power via 2 AA batteries
// - N20 geared motor driven by TB6612FNG I2C motor shield (D1 to GPIO22, D2 to GPIO21)
//   - power via 9V block battery
// see README.md and folder dev for more details/examples

// TB6612FNG shield's STM32F030 flashed with fixed firmware from https://hackaday.io/project/18439-motor-shield-reprogramming

// power consumption (motor at 9 V, ESP32+shield at ~3.2 V of 2 AA)
//   motor: ~40 mA no load
//   DOIT DEVKIT V1: 48.2 mA, 10 mA deep sleep
//   LOLIN D32: 39.4 mA, 0.7 mA deep sleep
//   bare ESP32: 36.8 mA, 5.2 uA deep sleep, with shield connected 16.9 uA?
//   TB6612FNG I2C shield: 2.8 mA, 1.8 mA in standby?


#include "WEMOS_Motor.h"

Motor M1(0x30, _MOTOR_B, 1000);

#define TOUCH_TH 50 // touch threshold, values usually 2-12 with finger depending on how hard the pinch, on batteries somehow higher
// with LOLIN D32 at pin 15 usually ~60, pin held to screw on the inside: ~33, when touching knob from outside: ~21

void setup()
{
  Serial.begin(115200);
  Serial.println("setup");
}

void loop()
{
  delay(100);
  // overview which pins are save to use: https://randomnerdtutorials.com/esp32-pinout-reference-gpios/
  // pins falling below threshold without touching: 32, 33
  int touch_open  = touchRead(12);
  int touch_close = touchRead(13); // if the first read drops below threshold, the second one will go to 0 as well after some time
  // sadly all touch pins report 0 every ~20 iterations without being touched, so we have to ignore 0
  // without touch the values are ~60-80, with touch 1-15
  bool do_open  = touch_open && touch_open < TOUCH_TH;
  bool do_close = touch_close && touch_close < TOUCH_TH;
  if (do_open || do_close)
    Serial.printf("touch: open %d, close %d\n", touch_open, touch_close);
  uint8_t cmd;
  if (do_open && do_close) cmd = _STANDBY;
  else if (do_open) cmd = _CCW;
  else if (do_close) cmd = _CW;
  else cmd = _STOP; // _SHORT_BRAKE
  M1.setmotor(cmd);

  if (cmd == _STANDBY) {
    esp_sleep_enable_timer_wakeup(20 * 1000 * 1000);
    esp_deep_sleep_start();
  }
}
