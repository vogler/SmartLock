// 3D printed smart lock using:
// - bare ESP32 (WROOM-32)
//   - power via 2 AA batteries
// - N20 geared motor driven by TB6612FNG I2C motor shield
//   - power via 9V block battery
// see folder dev/ for more details

#include "WEMOS_Motor.h"

Motor M1(0x30, _MOTOR_B, 1000);

#define TOUCH_TH 10 // touch threshold, values usually 2-8 with finger depending on how hard the pinch

void setup()
{
  Serial.begin(250000);
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
  if (do_open && do_close) cmd = _SHORT_BRAKE;
  else if (do_open) cmd = _CCW;
  else if (do_close) cmd = _CW;
  else cmd = _STOP;
  M1.setmotor(cmd);

//  esp_sleep_enable_timer_wakeup(5000 * 1000);
//  esp_deep_sleep_start();
}
