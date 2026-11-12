#include <Arduino.h>

const int lightPin = A0;
const int ledPin = 9;

// 这块光敏模块：环境越暗，ADC 读数越大。
const int turnOnAt = 600;
const int turnOffAt = 450;
const unsigned long sampleInterval = 100;
const unsigned long reportInterval = 1000;

bool ledOn = false;
int lightRaw = 0;
unsigned long lastSample = 0;
unsigned long lastReport = 0;

void updateNightLight() {
  lightRaw = analogRead(lightPin);

  if (!ledOn && lightRaw >= turnOnAt) {
    ledOn = true;
    digitalWrite(ledPin, LOW);
  } else if (ledOn && lightRaw <= turnOffAt) {
    ledOn = false;
    digitalWrite(ledPin, HIGH);
  }
  // 450 和 600 之间不改变灯的状态。
}

void printStatus() {
  Serial.print(F("A0 ADC: "));
  Serial.print(lightRaw);
  Serial.print(F(" | LED: "));
  Serial.println(ledOn ? F("ON") : F("OFF"));
}

void setup() {
  digitalWrite(ledPin, HIGH);
  pinMode(ledPin, OUTPUT);
  pinMode(lightPin, INPUT);
  Serial.begin(9600);
  Serial.println(F("Night light: on >= 600, off <= 450"));
  updateNightLight();
  printStatus();
}

void loop() {
  unsigned long now = millis();
  if (now - lastSample >= sampleInterval) {
    lastSample = now;
    updateNightLight();
  }
  if (now - lastReport >= reportInterval) {
    lastReport = now;
    printStatus();
  }
}
