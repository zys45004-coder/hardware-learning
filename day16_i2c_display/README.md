# 第十六天：用 I2C 屏幕显示光控夜灯状态

Arduino Uno / PlatformIO / Wokwi，沿用第十五天光敏模块（AO → A0）和低电平点亮 LED（阴极 → D9）。仍每 100 ms 采样；读数达到 600 开灯，降到 450 关灯，中间保持原状态。

## 屏幕接线与代码

新增 LCD1602，使用 I2C 模式，7 位地址为 0x27。VCC → 5V、GND → GND、SDA → A4、SCL → A5。
SDA 传数据，SCL 提供时钟，地址用于选中屏幕。LiquidCrystal_I2C 库负责通过 Wire 发送命令和字符。

- lcd.init() 初始化屏幕，lcd.backlight() 打开背光。
- lcd.setCursor(列, 行) 选位置，坐标从 0 起。第一行显示 ADC，第二行显示外接 LED 状态。
- 启动和之后每隔 1 秒，屏幕和串口显示同一份采样结果。刷新不清屏；覆盖旧文字，避免残留。

## 运行与验收记录

PlatformIO Build → SUCCESS → Ctrl+Shift+P → Wokwi: Start Simulator。

1. 启动（400 lux）：屏幕第一行约为 A0 ADC: 281，第二行 LED: OFF。
2. 调到约 50 lux：读数超过 600，外接灯点亮，屏幕约 1 秒内显示 LED: ON。
3. 调回约 400 lux：外接灯熄灭，屏幕显示 LED: OFF。
4. 先从亮处调到约 100 lux，再从暗处调到约 100 lux：ADC 都落在迟滞区间，屏幕分别显示保持灭、保持亮。
5. 补充检查：亮度从很暗调到很亮，确认较短 ADC 数字不会带上旧数字（未单独记录仿真结果）。

学习者确认 LCD 状态与外接 LED 一致：约 100 lux 时，从明处调来保持 OFF，从暗处调来保持 ON。表述中的 ADC 数字为参考值。PlatformIO 编译成功（RAM 448/2048 字节，Flash 5170/32256 字节）；归档时未重新运行仿真。

## 代码阅读笔记

- Uno R3 的硬件 I2C 使用 A4/SDA 与 A5/SCL；屏幕地址 0x27 用来在总线上区分设备，与引脚编号无关。
- lcd.init() 初始化显示控制器；lcd.backlight() 控制屏幕背光，与 D9 夜灯独立。
- lcd.print() 写完会移动光标。ADC 数字从 1023 变为 213 时，紧接数字写入的第一个空格会覆盖旧数字的末位；ON 后的空格同样用于覆盖 OFF 的最后一个 F。
