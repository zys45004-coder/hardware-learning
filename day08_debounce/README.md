# Day 08：millis() 按键消抖与串口联合控制

## 成果

将第六天的 delay(20) 消抖替换为 millis() 时间差判断。按键输入连续稳定至少 20 毫秒后才确认新状态，确认按下时切换 LED；长按不重复触发，等待稳定期间继续处理串口。

环境：Arduino Uno、Arduino 框架、VS Code、PlatformIO、Wokwi。
串口波特率 9600；1 开灯、0 关灯、? 查询，回车换行忽略，其他字符提示 Unknown command。
本版 LED 不自动闪烁，也不打印第七天的 Tick。

## 工程文件与运行

- src/main.cpp：完整程序。
- platformio.ini：uno 环境、atmelavr 平台、Arduino 框架。
- diagram.json：元件及接线。
- wokwi.toml：引用编译后的 firmware.hex 和 firmware.elf。
- .gitignore：忽略编译产物等生成文件。

1. 在 VS Code 中打开本目录，根目录应直接包含 platformio.ini。
2. PlatformIO → PROJECT TASKS → uno → General → Build，等待 SUCCESS。
3. Ctrl+Shift+P → Wokwi: Start Simulator。
4. 修改代码后保存、重新 Build，再启动仿真。

.pio/build/uno/firmware.hex 和 firmware.elf 由编译生成，不上传 .pio。

## 接线

- POWER 5V → 1000Ω 电阻 → LED 阳极 A；阴极 C → D8。
- D8 LOW 亮、HIGH 灭。
- D2 → 按键 1.l；GND → 按键 2.r（2.l 与 2.r 内部连通）。
- INPUT_PULLUP：松开 HIGH、按下 LOW。

## 消抖原理

三个变量各有用途：

| 变量 | 含义 |
| --- | --- |
| lastReading | 上一轮直接读到的原始电平 |
| stableButtonState | 已通过消抖确认的稳定状态 |
| lastChangeMillis | 最近一次检测到原始电平变化的时刻 |

每次 reading != lastReading，就将 lastChangeMillis 更新为当前时刻。
只有 now - lastChangeMillis >= 20 时，才检查并接受新的稳定状态。
确认状态变为 LOW 时切换灯；确认松开 HIGH 时仅更新状态。
持续按住时 reading 与 stableButtonState 都为 LOW，不再重复触发。

示例：100 ms 变 LOW，103 ms 变 HIGH，106 ms 再变 LOW；若之后持续稳定，最早 126 ms 确认按下。若 115 ms 又变化，则重新计时，最早 135 ms 确认新状态。
这里通过每轮读取判断稳定，不使用 delay()，所以仍能处理串口。

### 课堂中的关键纠错

如果删掉 stableButtonState = reading，从初始松开、灯灭开始，即使持续按住，灯也不会切换：外层 LOW != HIGH 虽成立，但 stableButtonState 一直为 HIGH，内层 stableButtonState == LOW 始终不成立。不能只看外层条件就判断会反复亮灭。

## 验收记录

以下实验均由学习者在本次聊天中确认符合预期。

### 基础按键

1. 初始灯灭。
2. 按一下并松开，灯亮并保持。
3. 再按一下并松开，灯灭并保持。
4. 按住 3 秒只切换一次并保持亮；松开仍然亮，按下日志只输出一次。

### 串口与按键联合控制

1. 发送 0，灯灭。
2. 按住按键，灯亮且只打印一次 Button pressed。
3. 继续按住并发送 0，灯灭，持续按住不会再次点亮。
4. 仍然按住并发送 ?，返回 Status: LED OFF。
5. 松开，等待一小会，再按一下，灯亮。

### 编译记录

本地工程 E:\day08_debounce 已由助手使用 PlatformIO 编译成功（SUCCESS）；RAM 291/2048 字节，Flash 2366/32256 字节。
随后由学习者启动仿真并反馈上述实验通过；本次归档没有重新运行仿真。

## 学习进度与来源

已掌握时间差消抖、稳定状态与原始读取值的区别、长按只触发一次，以及消抖期间继续处理串口。
代码和配置复制自本地 E:\day08_debounce；说明和验收依据本次聊天整理。
