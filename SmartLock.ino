// 3D printed smart lock using:
// - bare ESP32 (WROOM-32)
//   - power via 2 AA batteries
// - N20 geared motor driven by TB6612FNG (see lib/ and dev/ for I2C motor shield version)
//   - power via 9V block battery
// see README.md and folder dev for more details/examples

// power consumption (motor at 9 V, ESP32+shield at ~3.2 V of 2 AA)
//   motor: ~40 mA no load
//   DOIT DEVKIT V1: 48.2 mA, 10 mA deep sleep
//   LOLIN D32: 39.4 mA, 0.7 mA deep sleep
//   bare ESP32: 36.8 mA, 5.2 uA deep sleep, with shield connected 16.9 uA?
//   TB6612FNG I2C shield: 2.8 mA, 1.8 mA in standby?
//   TB6612FNG bare: ?

// #include "WEMOS_Motor.h"
// Motor M1(0x30, _MOTOR_B, 1000);
// changed from I2C shield to controlling TB6612FNG directly:
#define PIN_STBY 26
#define PIN_BIN1 27
#define PIN_BIN2 14
// PIN_PWM is pulled high directly since we do not need it

// see table in TB6612FNG.pdf on page 4
void motor(const byte in1, const byte in2, const byte stdby){
  digitalWrite(PIN_BIN1, in1);
  digitalWrite(PIN_BIN2, in2);
  digitalWrite(PIN_STBY, stdby);
}
void motor_brake(){ motor(HIGH, HIGH, HIGH); }
void motor_CCW(){   motor(LOW, HIGH, HIGH); }
void motor_CW(){    motor(HIGH, LOW, HIGH); }
void motor_stop(){  motor(LOW, LOW, HIGH); }
void motor_standby(){
  digitalWrite(PIN_STBY, LOW);
}

#define TOUCH_TH 50 // touch threshold, values usually 2-12 with finger depending on how hard the pinch, on batteries somehow higher
// with LOLIN D32 at pin 15 usually ~60, pin held to screw on the inside: ~33, when touching knob from outside: ~21

bool touch1 = false;
bool touch2 = false;
void touch1_ISR(){ touch1 = true; }
void touch2_ISR(){ touch2 = true; }

#define SLEEP_TIMEOUT 5000 // go to deep sleep after 5s of no action

#include "WiFi.h"

void setup()
{
  Serial.begin(115200);
  Serial.println("setup");

  pinMode(PIN_BIN1, OUTPUT);
  pinMode(PIN_BIN2, OUTPUT);
  pinMode(PIN_STBY, OUTPUT);

  // use touch interrupts instead of touchRead in loop because it sometimes triggers without touch (maybe if reading too often?)
  touchAttachInterrupt(T2, touch1_ISR, TOUCH_TH);
  touchAttachInterrupt(T3, touch2_ISR, TOUCH_TH);

  esp_sleep_enable_touchpad_wakeup();

  // BT and WiFi off
  btStop();
  WiFi.mode(WIFI_OFF);
}

unsigned long last_action;

void loop()
{
  // overview which pins are save to use: https://randomnerdtutorials.com/esp32-pinout-reference-gpios/
  // pins falling below threshold without touching: 32, 33
  // const byte touch_open  = touchRead(2);
  // const byte touch_close = touchRead(15); // if the first read drops below threshold, the second one will go to 0 as well after some time
  // sadly all touch pins report 0 every ~20 iterations without being touched, so we have to ignore 0
  // without touch the values are ~60-80, with touch 1-15
  // bool do_open  = touch_open && touch_open < TOUCH_TH;
  // bool do_close = touch_close && touch_close < TOUCH_TH;
  // if (do_open || do_close)
    // Serial.printf("touch: open %d, close %d\n", touch_open, touch_close);

  delay(100);
  if(touch1) Serial.println("Touch 1 detected");
  if(touch2) Serial.println("Touch 2 detected");
  bool do_open = touch1;
  bool do_close = touch2;
  touch1 = false; touch2 = false;

  if (do_open || do_close) last_action = millis();
  else if (millis() - last_action > SLEEP_TIMEOUT) {
    Serial.printf("Going to sleep because there was no action for %d ms\n", SLEEP_TIMEOUT);
    esp_deep_sleep_start();
  }

  if (do_open && do_close) {
    motor_standby();
    esp_sleep_enable_timer_wakeup(20 * 1000 * 1000);
    Serial.println("Going to sleep for 20s because you told me to.");
    esp_deep_sleep_start();
  }
  else if (do_open) motor_CCW();
  else if (do_close) motor_CW();
  else motor_standby();
}
