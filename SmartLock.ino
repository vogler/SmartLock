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

#include <MyConfig.h> // credentials, servers, ports

#include "WiFi.h"
WiFiClient wifi;

#define MQTT_TOPIC "door/auth"
bool auth = false;

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  Serial.printf("MQTT message on topic %s with payload ", topic);
  for(int i = 0; i < length; i++){
    Serial.print(*(payload+i));
  }
  auth = *payload == 1;
}

#include <PubSubClient.h>
PubSubClient mqtt(MQTT_SERVER, MQTT_PORT, mqtt_callback, wifi);

void setup_wifi() {
  delay(5);
  Serial.printf("Connecting to AP %s", WIFI_SSID);
  const unsigned long start_time = millis();
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  for (int i = 0; WiFi.waitForConnectResult() != WL_CONNECTED && i < 20; i++) {
    WiFi.begin(WIFI_SSID, WIFI_PASS); // for ESP32 also had to be moved inside the loop, otherwise only worked on every second boot, https://github.com/espressif/arduino-esp32/issues/2501#issuecomment-548484502
    delay(100);
    Serial.print(".");
  }
  const float connect_time = (millis() - start_time) / 1000.;
  Serial.println();
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.printf("Failed to connect to Wifi in %.3f seconds. Going to restart!", connect_time);
    ESP.restart();
  }
  Serial.printf("Connected in %.3f seconds. IP address: ", connect_time);
  Serial.println(WiFi.localIP());
}

void setup_mqtt() {
  mqtt.setCallback(mqtt_callback);
  randomSeed(micros());
  while (!mqtt.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "SmartLock-ESP32-" + String(random(0xffff), HEX);
    if (mqtt.connect(clientId.c_str())) {
      Serial.printf("connected as %s to mqtt://%s\n", clientId.c_str(), MQTT_SERVER);
      while(!mqtt.subscribe(MQTT_TOPIC)) Serial.print(".");
      Serial.printf("subscribed to topic %s\n", MQTT_TOPIC);
    } else {
      Serial.printf("failed, rc=%d. retry in 1s.\n", mqtt.state());
      delay(1000);
    }
  }
}

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

  setup_wifi();
  setup_mqtt();
}

void deep_sleep() {
  // BT and WiFi off; not sure if needed...
  btStop();
  // WiFi.mode(WIFI_OFF);
  WiFi.disconnect(true); // true = WiFi off
  esp_deep_sleep_start();
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
    deep_sleep();
  }

  if (do_open && do_close) {
    motor_standby();
    esp_sleep_enable_timer_wakeup(20 * 1000 * 1000);
    Serial.println("Going to sleep for 20s because you told me to.");
    deep_sleep();
  }
  else if (auth && do_open) motor_CCW();
  else if (auth && do_close) motor_CW();
  else motor_standby();
}
