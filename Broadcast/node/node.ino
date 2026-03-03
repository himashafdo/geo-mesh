#include <Wire.h>

void setup() {
  Serial.begin(115200);
  Wire.begin(4, 5);
  Serial.println("Scanning I2C...");
}

void loop() {
  byte error, address;

  for(address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("Found at 0x");
      Serial.println(address, HEX);
    }
  }

  delay(5000);
}