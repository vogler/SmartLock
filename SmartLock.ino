// 3D printed smart lock using:
// - bare ESP32 (WROOM-32)
//   - power via 2 AA batteries
// - N20 geared motor driven by TB6612FNG I2C motor shield (D1 to GPIO22, D2 to GPIO21)
//   - power via 9V block battery
// see folder dev/ for more details

// TB6612FNG shield's STM32F030 flashed with fixed firmware from https://hackaday.io/project/18439-motor-shield-reprogramming

// power consumption (motor at 9 V, rest at ~3.2 V of 2 AA)
//   motor: ~40 mA no load
//   DOIT DEVKIT V1: 48.2 mA, 10 mA deep sleep
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
  int touch_open  = touchRead(15);
  int touch_close = touchRead(4); // 0 once every ~22 iterations, later use D13 instead which seems fine
  // D2 should be touch but is always 0
  Serial.printf("touch: open %d, close %d, D13 %d\n", touch_open, touch_close, touchRead(13));
  // there were random touch values of 0 every couple of seconds for pin 4
  bool do_open  = touch_open && touch_open < TOUCH_TH;
  bool do_close = touch_close && touch_close < TOUCH_TH;
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
