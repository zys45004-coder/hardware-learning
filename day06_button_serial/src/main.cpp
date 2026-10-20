#include <Arduino.h>

const int ledPin = 8;
const int buttonPin = 2;

bool ledOn = false;
int lastButtonState = HIGH;

// 统一设置 LED 的状态、电平和串口提示
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
  // 先设置为灭灯电平，再配置为输出
  digitalWrite(ledPin, HIGH);
  pinMode(ledPin, OUTPUT);

  pinMode(buttonPin, INPUT_PULLUP);

  Serial.begin(9600);
  Serial.println("Ready: button / 1=ON / 0=OFF / ?=status");

  setLed(false);
}

void loop() {
  // 一、处理串口命令
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

  // 二、检测按键
  int buttonState = digitalRead(buttonPin);

  // HIGH → LOW：刚刚按下
  if (lastButtonState == HIGH && buttonState == LOW) {
    delay(20);
    buttonState = digitalRead(buttonPin);

    if (buttonState == LOW) {
      Serial.println("Button pressed");
      setLed(!ledOn);
    }
  }

  // LOW → HIGH：刚刚松开，做简单消抖
  if (lastButtonState == LOW && buttonState == HIGH) {
    delay(20);
    buttonState = digitalRead(buttonPin);
  }

  lastButtonState = buttonState;
}