// control N20 geared motor with L293D on ESP32
// used 9V block for motor
// with 5V from USB for logic the L293D works fine, with 3.3V it only turned the motor in one direction...
// used step-down from 9V to 5V for L293D logic and 2 AA (~3.2V) to power ESP32

// power consumption on 3.2V from 2 AA:
// - DOIT ESP32 DEVKIT V1:
//   48.2 mA with motor running, 10.0 mA in deep sleep :(
// - bare ESP32 on white PCB (used board FireBeetle-ESP32 in Arduino):
//   36.8 mA w/o anything connected, 5.2 uA deep sleep (measured with Voltcraft VC275TRMS with 3.3V from Rigol DP832 b/c my ANENG AN8009 at home couldn't deal with the current on the uA plug despite saying 200mA max)

const int en1 = 25;	// that is the pin that we use to control the motor's speed
const int in1 = 32;	// this is the pin that we use to tell the motor to go forward
const int in2 = 33;	// this is the pin that we use to tell the motor to go reverse

void setup()
{
  Serial.begin(250000);
  //  pinMode(LED_BUILTIN, OUTPUT); // debug light to check ESP32 is running alright
  pinMode(en1, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
}

void loop()
{
  //  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(en1, HIGH);
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  delay(2000);
  //  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  delay(2000);
  digitalWrite(en1, LOW);
  //  delay(3000);
  esp_sleep_enable_timer_wakeup(5000 * 1000); // 5s deep sleep
  esp_deep_sleep_start();
}
