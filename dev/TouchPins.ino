// loops over the touch pins and prints if value is below threshold
// all pins sometimes yield 0 without being touched -> have to ignore 0
// overview which pins are save to use: https://randomnerdtutorials.com/esp32-pinout-reference-gpios/

#define TOUCH_TH 50 // touch threshold, values usually 2-12 with finger depending on how hard the pinch, on batteries somehow higher
// with LOLIN D32 on pin 15 usually ~60, pin held to screw on the inside of the lock: ~33, when touching knob from outside: ~21

void setup()
{
  Serial.begin(115200);
  Serial.println("setup");
}

#define LEN(a) sizeof(a)/sizeof(a[0])

void loop()
{
  byte p[] = { 4, 2, 15, 13, 12, 14, 27, 33, 32 }; // removed pin 0 (Touch1) because it is always 1
  for (int i = 0; i < LEN(p); i++) {
//    Serial.printf("Touch%d (pin %d) = %d, ", i, p[i], touchRead(p[i]));
    int v = touchRead(p[i]);
    if (v && v < 60)
      Serial.printf("Touch%d (pin %d) = %d\n", i, p[i], v);
  }
//  Serial.println();
}
