#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <RTClib.h>

constexpr uint8_t lightPin = A0;
constexpr uint8_t ledPin = 9;
constexpr uint8_t lcdAddress = 0x27;
constexpr uint8_t nightStart = 18;  // 18:00 包含
constexpr uint8_t nightEnd = 6;     // 06:00 不包含
constexpr int turnOnAt = 600;
constexpr int turnOffAt = 450;
constexpr unsigned long lightInterval = 100;
constexpr unsigned long clockInterval = 1000;

LiquidCrystal_I2C lcd(lcdAddress, 16, 2);
RTC_DS1307 rtc;
DateTime clockSnapshot;
bool lcdReady = false;
bool rtcReady = false;
bool clockValid = false;
bool nightAllowed = false;
bool ledOn = false;
int lightRaw = 0;
unsigned long lastLight = 0;
unsigned long lastClock = 0;

bool isNight(uint8_t hour) {
  // 时间段跨午夜：傍晚到午夜，或午夜到早晨。
  return hour >= nightStart || hour < nightEnd;
}

void readClock() {
  clockValid = false;
  if (rtcReady) {
    Wire.clearWireTimeoutFlag();
    clockSnapshot = rtc.now();
    clockValid = !Wire.getWireTimeoutFlag() && clockSnapshot.isValid();
  }
  nightAllowed = clockValid && isNight(clockSnapshot.hour());
}

void updateLight() {
  lightRaw = analogRead(lightPin);

  if (!nightAllowed) {
    ledOn = false;
  } else if (!ledOn && lightRaw >= turnOnAt) {
    ledOn = true;
  } else if (ledOn && lightRaw <= turnOffAt) {
    ledOn = false;
  }

  digitalWrite(ledPin, ledOn ? LOW : HIGH);
}

void showStatus() {
  char time[] = "hh:mm:ss";
  if (clockValid) {
    clockSnapshot.toString(time);
    Serial.print(time);
  } else {
    Serial.print(F("RTC unavailable"));
  }
  Serial.print(nightAllowed ? F(" | NIGHT") : F(" | BLOCKED"));
  Serial.print(F(" | ADC: "));
  Serial.print(lightRaw);
  Serial.print(F(" | LED: "));
  Serial.println(ledOn ? F("ON") : F("OFF"));

  if (!lcdReady) return;
  lcd.setCursor(0, 0);
  if (clockValid) {
    lcd.print(time);
    lcd.print(nightAllowed ? F(" NIGHT  ") : F(" DAY    "));
  } else {
    lcd.print(F("RTC unavailable "));
  }
  lcd.setCursor(0, 1);
  lcd.print(F("A0:"));
  if (lightRaw < 1000) lcd.print(' ');
  if (lightRaw < 100) lcd.print(' ');
  if (lightRaw < 10) lcd.print(' ');
  lcd.print(lightRaw);
  lcd.print(ledOn ? F(" LED:ON  ") : F(" LED:OFF "));
}

void setup() {
  digitalWrite(ledPin, HIGH);
  pinMode(ledPin, OUTPUT);
  pinMode(lightPin, INPUT);
  Serial.begin(9600);
  Wire.begin();
  Wire.setClock(100000);
  Wire.setWireTimeout(25000, true);

  Wire.beginTransmission(lcdAddress);
  lcdReady = Wire.endTransmission() == 0;
  if (lcdReady) {
    lcd.init();
    lcd.backlight();
  }
  rtcReady = rtc.begin() && rtc.isrunning();

  readClock();
  updateLight();
  showStatus();
  lastLight = lastClock = millis();
}

void loop() {
  const unsigned long now = millis();
  const bool refreshClock = now - lastClock >= clockInterval;

  if (refreshClock) {
    lastClock = now;
    readClock();
  }
  if (refreshClock || now - lastLight >= lightInterval) {
    lastLight = now;
    updateLight();
  }
  if (refreshClock) {
    showStatus();
  }
}
