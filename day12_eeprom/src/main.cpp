#include <Arduino.h>
#include <EEPROM.h>
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
const unsigned long defaultInterval = 1000;
const unsigned long minInterval = 100;
const unsigned long maxInterval = 10000;
const int intervalAddress = 0;
unsigned long ledInterval = defaultInterval;

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
  Serial.print(ledOn ? "ON" : "OFF");
  Serial.print(" | Interval: ");
  Serial.print(ledInterval);
  Serial.println(" ms");
}

// 检查完整参数，只接受数字，且数值必须在 100～10000 ms 内
bool parseInterval(const char *text, unsigned long &result) {
  if (text[0] == '\0') {
    return false;
  }

  unsigned long value = 0;
  for (int i = 0; text[i] != '\0'; i++) {
    if (text[i] < '0' || text[i] > '9') {
      return false;
    }
    // 字符 '0'～'9' 转换成数字 0～9
    value = value * 10 + (text[i] - '0');
    // 每读一位就检查，避免超长数字累积溢出
    if (value > maxInterval) {
      return false;
    }
  }

  if (value < minInterval) {
    return false;
  }
  result = value;
  return true;
}

void loadInterval() {
  unsigned long savedInterval = 0;
  EEPROM.get(intervalAddress, savedInterval);

  if (savedInterval >= minInterval && savedInterval <= maxInterval) {
    ledInterval = savedInterval;
    Serial.print("Loaded from EEPROM: ");
  } else {
    ledInterval = defaultInterval;
    Serial.print("Using default interval: ");
  }
  Serial.print(ledInterval);
  Serial.println(" ms");
}

void saveInterval() {
  EEPROM.put(intervalAddress, ledInterval);
  Serial.print("Saved to EEPROM: ");
  Serial.print(ledInterval);
  Serial.println(" ms");
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
  } else if (strncmp(command, "interval ", 9) == 0) {
    unsigned long newInterval = 0;
    // command + 9 跳过前面的 interval 和一个空格
    if (parseInterval(command + 9, newInterval)) {
      ledInterval = newInterval;
      previousLedMillis = millis();
      Serial.print("Interval: ");
      Serial.print(ledInterval);
      Serial.println(" ms");
    } else {
      Serial.println("Invalid interval: use 100-10000 ms");
    }
  } else if (strcmp(command, "interval") == 0) {
    Serial.println("Invalid interval: use 100-10000 ms");
  } else if (strcmp(command, "save") == 0) {
    saveInterval();
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
  Serial.println("Ready: on / off / auto / status / interval N / save");
  loadInterval();
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

