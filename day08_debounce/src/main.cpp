#include <Arduino.h>

const int ledPin = 8;
const int buttonPin = 2;

bool ledOn = false;

// 上一次直接读到的电平
int lastReading = HIGH;

// 已经通过消抖确认的稳定状态
int stableButtonState = HIGH;

// 最近一次读到电平变化的时刻
unsigned long lastChangeMillis = 0;
const unsigned long debounceInterval = 20;

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
  pinMode(buttonPin, INPUT_PULLUP);

  Serial.begin(9600);
  Serial.println("Ready: button / 1=ON / 0=OFF / ?=status");
  setLed(false);
}

void loop() {
  // 一、处理串口
  if (Serial.available() > 0) {
    char command = Serial.read();
    if (command == '1') {
      setLed(true);
    } else if (command == '0') {
      setLed(false);
    } else if (command == '?') {
      Serial.print("Status: ");
      Serial.println(ledOn ? "LED ON" : "LED OFF");
    } else if (command != '\r' && command != '\n') {
      Serial.println("Unknown command");
    }
  }

  // 二、读取按键并记录变化时刻
  unsigned long now = millis();
  int reading = digitalRead(buttonPin);
  if (reading != lastReading) {
    lastChangeMillis = now;
  }

  // 三、连续稳定至少 20 毫秒后，才认可新状态
  if (now - lastChangeMillis >= debounceInterval) {
    if (reading != stableButtonState) {
      stableButtonState = reading;
      if (stableButtonState == LOW) {
        Serial.println("Button pressed");
        setLed(!ledOn);
      }
    }
  }

  lastReading = reading;
}
