# DIY SmartLock
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
      - authentification fast enough to do it only after waking up?
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