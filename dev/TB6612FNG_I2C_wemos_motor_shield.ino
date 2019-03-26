// WEMOS I2C Dual Motor Shield (TB6612FNG)
// https://hobbycomponents.com/shields/869-wemos-d1-mini-motor-drive-shield
// Connected D1 to GPIO22, D2 to GPIO21. Wasn’t detected by i2c_scanner.
// Flashed fixed firmware according to
// https://hackaday.io/project/18439-motor-shield-reprogramming
// Afterwards detected at I2C address 0x30, but motor doesn’t move with any sketch.
// Found out that Standby pin of the IC must be pulled high, but it's not connected to anything.
// There are solder pads on the back for I2C and IO (control via extra pin S).
// Docs said I2C is the default, but apparently not for this clone.
// After soldering bridge STBY I2C, it worked.

#include "WEMOS_Motor.h"

Motor M1(0x30, _MOTOR_B, 1000);

void setup()
{
  Serial.begin(250000);
  Serial.println("setup");
}

float pwm;
void loop()
{
  Serial.printf("\r\nTest PWM 30 to 100, step 0.5,CW\r\n");
  for (pwm = 30; pwm <= 100; pwm += 0.5)
  {
    M1.setmotor( _CW, pwm);
    Serial.println(pwm);
  }

  M1.setmotor(_STOP);

  Serial.println("Motor B STOP");
  delay(200);

  Serial.printf("Test PWM 30 to 100, step 0.5,CCW\r\n");
  for (pwm = 30; pwm <= 100; pwm += 0.5)
  {
    M1.setmotor(_CCW, pwm);
    Serial.println(pwm);
  }

  M1.setmotor(_STOP);

  Serial.println("Motor B STOP");
  delay(200);
  M1.setmotor(_CW, 50); delay(500);

//  esp_sleep_enable_timer_wakeup(5000 * 1000);
//  esp_deep_sleep_start();
}
