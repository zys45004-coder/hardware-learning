#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <RTClib.h>

constexpr uint8_t lcdAddress = 0x27;
constexpr unsigned long refreshInterval = 1000;

LiquidCrystal_I2C lcd(lcdAddress, 16, 2);
RTC_DS1307 rtc;

bool lcdReady = false;
bool rtcReady = false;
unsigned long lastRefresh = 0;

void scanI2c() {
  uint8_t found = 0;
  Serial.println(F("Scanning I2C (0x08-0x77)..."));
  for (uint8_t address = 0x08; address <= 0x77; ++address) {
    Wire.beginTransmission(address);
    uint8_t result = Wire.endTransmission();
    if (result == 0) {
      Serial.print(F("Found device at 0x"));
      if (address < 0x10) Serial.print('0');
      Serial.println(address, HEX);
      if (address == lcdAddress) lcdReady = true;
      ++found;
    } else if (result != 2) {
      Serial.print(F("I2C error: "));
      Serial.println(result);
      if (result == 5) break;
    }
  }
  Serial.print(F("Devices found: "));
  Serial.println(found);
}

void showClock() {
  // 读取同一份快照，再拆成年月日和时分秒。
  DateTime now = rtc.now();
  if (!now.isValid()) {
    Serial.println(F("RTC time invalid"));
    if (lcdReady) {
      lcd.setCursor(0, 0);
      lcd.print(F("RTC time invalid"));
      lcd.setCursor(0, 1);
      lcd.print(F("                "));
    }
    return;
  }
  char date[] = "YYYY-MM-DD";
  char time[] = "hh:mm:ss";
  now.toString(date);
  now.toString(time);
  Serial.print(date);
  Serial.print(' ');
  Serial.println(time);
  if (lcdReady) {
    lcd.setCursor(0, 0);
    lcd.print(date);
    lcd.print(F("      "));
    lcd.setCursor(0, 1);
    lcd.print(time);
    lcd.print(F("        "));
  }
}

void setup() {
  Serial.begin(9600);
  Wire.begin();
  Wire.setClock(100000);
  Wire.setWireTimeout(25000, true);
  scanI2c();

  if (lcdReady) {
    lcd.init();
    lcd.backlight();
  }

  if (!rtc.begin()) {
    Serial.println(F("RTC not found"));
    if (lcdReady) lcd.print(F("RTC not found"));
    return;
  }
  if (!rtc.isrunning()) {
    Serial.println(F("RTC stopped"));
    if (lcdReady) lcd.print(F("RTC stopped"));
    return;
  }

  rtcReady = true;
  showClock();
  lastRefresh = millis();
}

void loop() {
  unsigned long now = millis();
  if (rtcReady && now - lastRefresh >= refreshInterval) {
    lastRefresh = now;
    showClock();
  }
}
