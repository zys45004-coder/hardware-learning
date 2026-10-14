#include <Arduino.h>

const int ledPin = 8;

void setup()
{
    digitalWrite(ledPin, HIGH);  // 低电平点亮，初始熄灭
    pinMode(ledPin, OUTPUT);

    Serial.begin(9600);
    Serial.println("Ready: send 1=ON, 0=OFF");
}

void loop()
{
    // 检查是否有尚未读取的数据
    if (Serial.available() > 0)
    {
        // 每次读取一个字符
        char command = Serial.read();

        if (command == '\r' || command == '\n')
        {
            return; // 结束本次 loop()，框架随后会再次调用它
        }

        Serial.print("Received: ");
        Serial.print(command);

        Serial.print(" | Code: ");
        Serial.println((int)command);

        if (command == '1')
        {
            digitalWrite(ledPin, LOW);
            Serial.println("LED ON");
        }
        else if (command == '0')
        {
            digitalWrite(ledPin, HIGH);
            Serial.println("LED OFF");
        }
        else if (command == '?')
        {
            int level = digitalRead(ledPin);

            if (level == LOW)
            {
                Serial.println("Status: LED ON");
            }
            else
            {
                Serial.println("Status: LED OFF");
            }
        }
        else if (command == '\r' || command == '\n')
        {
            // 忽略发送时附带的回车和换行字符
        }
        else
        {
            Serial.println("Unknown command");
        }
    }
}