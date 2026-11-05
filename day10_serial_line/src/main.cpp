#include <Arduino.h>
#include <string.h>

const int ledPin = 8;
const int buttonPin = 2;

bool ledOn = false;
bool autoMode = false;

// 最多接收 31 个命令字符，另留一个位置给字符串结束符
const int commandCapacity = 32;
char commandBuffer[commandCapacity];
int commandLength = 0;
bool commandTooLong = false;

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

// 只在收到完整一行后执行
void executeCommand(const char *command) {
  if (strcmp(command, "on") == 0) {
    autoMode = false;
    Serial.println("Mode: MANUAL");
    setLed(true);
  } else if (strcmp(command, "off") == 0) {
    autoMode = false;
    Serial.println("Mode: MANUAL");
    setLed(false);
  } else if (strcmp(command, "auto") == 0) {
    autoMode = true;
    previousLedMillis = millis();
    Serial.println("Mode: AUTO");
  } else if (strcmp(command, "status") == 0) {
    printStatus();
  } else {
    Serial.println("Unknown command");
  }
}

void receiveSerial() {
  // 每轮最多读取一个字符，按键和计时仍继续运行
  if (Serial.available() > 0) {
    char received = Serial.read();

    // 同时兼容 Enter 发出的 CR、LF 和 CRLF
    if (received == '\r' || received == '\n') {
      if (commandTooLong || commandLength > 0) {
        Serial.println();
        if (commandTooLong) {
          Serial.println("Command too long");
        } else {
          commandBuffer[commandLength] = '\0';
          executeCommand(commandBuffer);
        }
      }
      // CRLF 的第二个字符和空行都不会重复执行命令
      commandLength = 0;
      commandTooLong = false;
    } else if (received == '\b' || received == 127) {
      // 普通输入允许退格；已超长的整行仍需丢弃
      if (!commandTooLong && commandLength > 0) {
        commandLength--;
        Serial.print("\b \b");
      }
    } else if (received >= 32 && received <= 126) {
      if (!commandTooLong) {
        if (commandLength < commandCapacity - 1) {
          commandBuffer[commandLength] = received;
          commandLength++;
          // 将收到的字符发回终端，让用户能看见输入
          Serial.write(received);
        } else {
          commandTooLong = true;
        }
      }
    }
  }
}

void setup() {
  digitalWrite(ledPin, HIGH);
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  Serial.begin(9600);
  Serial.println("Ready: on / off / auto / status (press Enter)");
  setLed(false);
  printStatus();
}

void loop() {
  // 一、接收并处理整行命令
  receiveSerial();

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


