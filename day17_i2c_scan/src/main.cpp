#include <Arduino.h>
#include <Wire.h>

void scanI2c() {
  uint8_t found = 0;
  uint8_t errors = 0;
  Serial.println(F("Scanning I2C (0x08-0x77)..."));

  // 跳过 I2C 保留地址。
  for (uint8_t address = 0x08; address <= 0x77; ++address) {
    Wire.beginTransmission(address);
    uint8_t result = Wire.endTransmission();

    if (result == 0) {
      Serial.print(F("Found device at 0x"));
      if (address < 0x10) Serial.print('0');
      Serial.println(address, HEX);
      ++found;
    } else if (result != 2) {
      ++errors;
      Serial.print(F("I2C error "));
      Serial.print(result);
      Serial.print(F(" at 0x"));
      Serial.println(address, HEX);
      if (result == 5) break; // 总线超时，停止本轮扫描
    }
  }

  Serial.print(F("Devices found: "));
  Serial.println(found);
  if (errors != 0) Serial.println(F("Scan had bus errors."));
}

void setup() {
  Serial.begin(9600);
  Wire.begin();
  Wire.setWireTimeout(25000, true);
  scanI2c();
  Serial.println(F("Send s to scan again."));
}

void loop() {
  if (Serial.available() > 0 && Serial.read() == 's') {
    scanI2c();
  }
}
