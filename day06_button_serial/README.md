# Day 06：按键与串口联合控制 LED

## 目标与环境

Arduino Uno，Arduino 框架，VS Code + PlatformIO + Wokwi。
按键与串口共用 ledOn，通过 setLed(bool on) 同时更新状态、D8 电平和串口日志。

## 接线

- POWER 5V → 1000Ω 电阻 → LED 阳极 A；LED 阴极 C → D8。
- LED 为低电平点亮：D8 LOW 亮，HIGH 灭。
- D2 → 按键 1.l；GND → 按键 2.r（同组 2.l 与 2.r 内部连通）。
- D2 使用 INPUT_PULLUP：松开 HIGH，按下 LOW。

## 操作

串口波特率 9600。发送 1 开灯，0 关灯，? 查询；忽略回车换行，其他字符提示 Unknown command。
每次按下按键切换一次，长按不重复切换。

## 运行

1. VS Code 打开本目录，确认根目录包含 platformio.ini。
2. PlatformIO → PROJECT TASKS → uno → General → Build，等待 SUCCESS。
3. Ctrl+Shift+P → Wokwi: Start Simulator。
4. 修改代码后保存并重新 Build，再启动仿真。

.pio 是编译产物，无需上传。编译后生成 .pio/build/uno/firmware.hex 和 firmware.elf，供 wokwi.toml 引用。

## 验收记录

以下八项已由学习者在本次聊天中确认符合预期：

1. 启动时灯灭。
2. 发送 1，灯亮。
3. 按一下并松开，灯灭。
4. 发送 ?，返回 Status: LED OFF。
5. 按一下并松开，灯亮。
6. 发送 0，灯灭。
7. 长按 3 秒，只切换一次并保持亮。
8. 松开后发送 a，提示 Unknown command，灯保持亮。

## 关键理解

- on 是本次函数调用的参数；ledOn 是保存的灯状态，ledOn = on 负责更新状态。
- setLed(!ledOn) 将当前状态取反。
- 串口发送一次 1 后，该字符被读取并消费；再按一次按键，灯会保持灭，直到下一次控制动作。
- loop 中先处理串口，再处理按键；这是代码处理顺序，不保证按真实到达时间排序。
- 本版保留 delay(20) 简单消抖，尚未改为 millis() 非阻塞消抖。

## 文件来源

代码和接线配置复制自本地 E:\day04_serial 的当前文件；虽然原文件夹名仍为第四天，代码内容已更新为第六天版本。本目录单独归档第六天内容。
验收记录来自学习者反馈；本次归档未重新编译或仿真。
