#include <Arduino.h>

const int ledPin = 8;
const int buttonPin = 2;

bool ledOn = false;
bool autoMode = false;

unsigned long previousLedMillis = 0;
const unsigned long ledInterval = 1000;

int lastReading = HIGH;
int stableButtonState = HIGH;
unsigned long lastChangeMillis = 0;
const unsigned long debounceInterval = 20;

void setLed(bool on) {
  ledOn = on;
  // 当前接线：LOW 亮，HIGH 灭
  digitalWrite(ledPin, ledOn ? LOW : HIGH);
  Serial.println(ledOn ? "LED ON" : "LED OFF");
}

void printStatus() {
  Serial.print("Mode: ");
  Serial.print(autoMode ? "AUTO" : "MANUAL");
  Serial.print(" | LED: ");
  Serial.println(ledOn ? "ON" : "OFF");
}

void setup() {
  digitalWrite(ledPin, HIGH);
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  Serial.begin(9600);
  Serial.println("Ready: 1=ON / 0=OFF / a=AUTO / ?=status");
  setLed(false);
  printStatus();
}

void loop() {
  // 一、串口命令
  if (Serial.available() > 0) {
    char command = Serial.read();
    if (command == '1') {
      autoMode = false;
      Serial.println("Mode: MANUAL");
      setLed(true);
    } else if (command == '0') {
      autoMode = false;
      Serial.println("Mode: MANUAL");
      setLed(false);
    } else if (command == 'a') {
      autoMode = true;
      // 保留当前亮灭状态，从现在起计时
      previousLedMillis = millis();
      Serial.println("Mode: AUTO");
    } else if (command == '?') {
      printStatus();
    } else if (command != '\r' && command != '\n') {
      Serial.println("Unknown command");
    }
  }

  unsigned long now = millis();

  // 二、连续稳定 20 ms 后确认按键状态
  int reading = digitalRead(buttonPin);
  if (reading != lastReading) {
    lastChangeMillis = now;
  }
  if (now - lastChangeMillis >= debounceInterval) {
    if (reading != stableButtonState) {
      stableButtonState = reading;
      if (stableButtonState == LOW) {
        autoMode = false;
        Serial.println("Button pressed");
        Serial.println("Mode: MANUAL");
        setLed(!ledOn);
      }
    }
  }
  lastReading = reading;

  // 三、自动模式下才定时切换
  if (autoMode) {
    if (now - previousLedMillis >= ledInterval) {
      previousLedMillis = now;
      setLed(!ledOn);
    }
  }
}
