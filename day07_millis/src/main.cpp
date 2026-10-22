#include <Arduino.h>

const int ledPin = 8;
bool ledOn = false;

// LED 的计时
unsigned long previousLedMillis = 0;
const unsigned long ledInterval = 3000;

// Tick 的计时
unsigned long previousTickMillis = 0;
const unsigned long tickInterval = 1000;

void setLed(bool on) {
  ledOn = on;

  if (ledOn) {
    digitalWrite(ledPin, LOW);
    Serial.println("LED ON");
  } else {
    digitalWrite(ledPin, HIGH);
    Serial.println("LED OFF");
  }
}

void setup() {
  digitalWrite(ledPin, HIGH);
  pinMode(ledPin, OUTPUT);

  Serial.begin(9600);
  Serial.println("Ready: send ? to query LED");
  setLed(false);

  unsigned long startMillis = millis();
  previousLedMillis = startMillis;
  previousTickMillis = startMillis;
}

void loop() {
  unsigned long now = millis();

  // 第一件事：每隔 3 秒切换 LED
  if (now - previousLedMillis >= ledInterval) {
    previousLedMillis = now;
    setLed(!ledOn);
  }

  // 第二件事：每隔 1 秒打印 Tick
  if (now - previousTickMillis >= tickInterval) {
    previousTickMillis = now;
    Serial.println("Tick");
  }

  // 第三件事：处理串口查询
  if (Serial.available() > 0) {
    char command = Serial.read();

    if (command == '?') {
      Serial.print("Status: ");
      Serial.println(ledOn ? "LED ON" : "LED OFF");
    } else if (command != '\r' && command != '\n') {
      Serial.println("Unknown command");
    }
  }
}