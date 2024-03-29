# DIY SmartLock
3D printed smart lock using ESP32 via 2 AA cells, N20 geared motor driven by TB6612FNG via 9V block.

[Fusion 360](https://a360.co/3NtIbpZ) offers download of STL files for 3D printing.
The slot for the key has some clearance. I wrapped some insulating tape around the end of the key to make a good fit.

The ESP32 goes into deep sleep and wakes up using the capacitive touch sensor on pin `T2` which is connected to the metal case around the lock. Since this is connected to the metal door knob on the outside of the door, it is enough to touch the door knob outside to wake up the ESP32 and open the door.
Whether it should open or not can be set via MQTT on `door/auth` which is checked via WiFi on wake.
This is pretty flexible with MQTT dashboard on the phone or some automation with presence detection in node-red on the RPi.

![image](https://user-images.githubusercontent.com/493741/160496199-4d856411-4438-4757-b90a-679956a14b8a.png)
[Album](https://photos.app.goo.gl/bewiZ1qH8sHnJjmg7) with more photos and videos of the lock in action.

## Log
1. Test which motor is strong enough
    - geared motor at 9V can close lock and open door
      - at 5V from power bank only enough to close and open the lock but not enough to pull the [trap](https://de.wikipedia.org/wiki/Schlossfalle) open
      - at 4.7V from LiPo already can't close lock anymore
2. Design frame for door, key and motor
    - used Fusion 360: file, stl on thingiverse.com
3. 3D print
    - used Ultimaker 2 with black filament
      - Cura settings: file
    - everything worked on the first try, fit perfectly and seems strong enough - lucky?
4. How to control? -> ESP32 (board or bare)
    - ESP8266's deep sleep current is too high and only has WiFi.
    - ESP32's deep sleep current is very low (depending on the board) and it also has bluetooth.
4. How to power? -> 2 AA and 9V block
    - 2*AA batteries ~= 3.2V for ESP32: alkaline < 5 Wh
    - 9V block battery for motor: alkaline < 3 Wh
    - could also step-down from 9V or step-up from 3.2V
      - step-up from 3.2V seems better since ESP is running more often than the motor and AA have higher capacity
      - converter efficiency?
      - constant current drain if we do not cut it off?
5. Test motor control
    - first tried L293D: only works at 5V (only one direction worked at 3.2V)
    - then tried TB6612FNG Wemos I2C motor shield: works and specs are more than enough (two motors, 2.7-5.5V logic, <15V motor, 1.2A const./3.2A peak per channel)
      - also have just the chip without a STM32 for I2C - probably needs less power (but some more cables), however the overhead of the shield seems low enough when put in standby
6. How to open door? -> touch door knob
    - constantly check via bluetooth if phone is close, and pull open if within certain distance (RSSI good enough?)
      - seems unreliable and energy-intense - how long can the motor pull it open w/o damage?
    - trigger it manually from phone via bluetooth/WiFi -> don't want to take phone out
    - detect touch of door knob and open then
      - ESP32 can stay in deep sleep and wake up from touch via one of its touch pins; metal on the inside is connected to the outside of the door
      - authentification fast enough to do it only after waking up? -> yes
      - works fine when powered via USB, but difference of no-touch vs. touch is very small when running on batteries (lower voltage, less ground capacitance?)
        - tried to increase accuracy by increasing measurement time
        - seems a bit better with USB cable hanging
        - found some threshold that works (see comments), but have to test if reliable under changing temperature, voltage etc.
7. How to authenticate?
    - not at all: everyone can open the door if they know they just have to touch it (which no one would probably do :))
    - have some touch Morse code: takes too long
    - via bluetooth or phone connected to WiFi:
      - needs some logic that it only opens after some time away
        - what if I'm out running without my phone?
    - bluetooth
      - ESP32 as client: need some server running on the phone
      - ESP32 as server that phone connects to
        - auto-connect doesn't seem to work, don't want to manually connect
      - BLE scan for set of devices: not encrypted, can be spoofed
        - phone does not advertise, would have to start e.g. via nRF Connect; but maybe can be automated
        - watch only advertises if not connected to phone
        - bluetooth beacons advertise but need to be turned on first
        - Stryd footpod: advertises when not in use only?
    - WiFi: do presence detection on the RPi and ask it whether to open
      - phone connected to WiFi (already connects at the elevator)
      - yes if sequence: door sensor open/close, no motion, phone disconnected from WiFi for x min, phone connected to WiFi


## Links
- [Youtube: Comparison of 10 ESP32 Battery powered Boards without display (incl. deep-sleep)](https://www.youtube.com/watch?v=-769_YIeGmI)
    - [ESP32 Boards Comparison](https://docs.google.com/spreadsheets/d/1Mu-bNwpnkiNUiM7f2dx8-gPnIAFMibsC2hMlWhIHbPQ/edit#gid=0): LOLIN D32: 125 uA deep sleep from battery
- [ESP32 API: Touch Sensor](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/touch_pad.html)
- [ESP32 Capacitive Sensors](https://nick.zoic.org/art/esp32-capacitive-sensors/): maybe the difference USB<>battery is because of the bigger ground plane with USB
- [Touch Sensor Application Note](https://github.com/espressif/esp-iot-solution/blob/e330b4d3ed947c7a99b448983096797e65be81c9/documents/touch_pad_solution/touch_sensor_design_en.md)
