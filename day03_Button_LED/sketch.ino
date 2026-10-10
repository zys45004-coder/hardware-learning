const int ledPin = 8;
const int buttonPin = 2;

bool ledOn = false;
int lastButtonState = HIGH;

void setup()
{
    digitalWrite(ledPin, HIGH);  // 初始熄灭
    pinMode(ledPin, OUTPUT);

    pinMode(buttonPin, INPUT_PULLUP);
}

void loop()
{
    int buttonState = digitalRead(buttonPin);

    // 从松开变成按下
    if (lastButtonState == HIGH && buttonState == LOW)
    {
        delay(20);  // 等待短暂的机械抖动过去

        // 再检查一次，确认仍处于按下状态
        buttonState = digitalRead(buttonPin);

        if (buttonState == LOW)
        {
            ledOn = !ledOn;  // 切换记忆状态

            if (ledOn)
            {
                digitalWrite(ledPin, LOW);   // 点亮
            }
            else
            {
                digitalWrite(ledPin, HIGH);  // 熄灭
            }
        }
    }

    // 松开也做一次简单确认
    if (lastButtonState == LOW && buttonState == HIGH)
    {
        delay(20);
        buttonState = digitalRead(buttonPin);
    }

    lastButtonState = buttonState;
}