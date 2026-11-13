#include <Arduino.h>
#include <Wire.h>

const uint8_t lcdAddress = 0x27;

void probeLcd() {
  Wire.beginTransmission(lcdAddress);
  const uint8_t result = Wire.endTransmission(); // 只发地址，不发送数据
  if (result == 0) {
    Serial.println(F("0x27: ACK"));
  } else if (result == 2) {
    Serial.println(F("0x27: NACK"));
  } else {
    Serial.print(F("I2C error: "));
    Serial.println(result);
  }
}

void setup() {
  Serial.begin(9600);
  Wire.begin();
  Wire.setClock(100000);
  Wire.setWireTimeout(25000, true);
  Serial.println(F("Day 18: I2C waveform"));
  probeLcd();
  Serial.println(F("Send s to probe once more."));
}

void loop() {
  if (Serial.available() > 0 && Serial.read() == 's') {
    probeLcd();
  }
}
