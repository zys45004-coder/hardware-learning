#include <Arduino.h>

const int ledPin = 8;
const int buttonPin = 2;

bool ledOn = false;
int lastButtonState = HIGH;
int pressCount = 0;  // 记录有效按下次数

void setup()
{
    digitalWrite(ledPin, HIGH);  // 初始熄灭
    pinMode(ledPin, OUTPUT);
    pinMode(buttonPin, INPUT_PULLUP);

    Serial.begin(9600);
    Serial.println("System ready");
    Serial.println("LED OFF");
}

void loop()
{
    int buttonState = digitalRead(buttonPin);

    if (lastButtonState == HIGH && buttonState == LOW)
    {
        delay(20);
        buttonState = digitalRead(buttonPin);

        if (buttonState == LOW)
        {
            ledOn = !ledOn;
            pressCount++;

            if (ledOn)
            {
                digitalWrite(ledPin, LOW);
            }
            else
            {
                digitalWrite(ledPin, HIGH);
            }

            // 输出次数与 LED 状态
            Serial.print("Press count: ");
            Serial.print(pressCount);
            Serial.print(" | LED: ");

            if (ledOn)
            {
                Serial.print("ON");
            }
            else
            {
                Serial.print("OFF");
            }

            Serial.print("| D8: ");
            Serial.println(digitalRead(ledPin) );
        }
    }

    if (lastButtonState == LOW && buttonState == HIGH)
    {
        delay(20);
        buttonState = digitalRead(buttonPin);
    }

    lastButtonState = buttonState;
}