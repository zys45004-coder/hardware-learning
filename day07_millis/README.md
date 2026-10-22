# Day 07：millis() 非阻塞计时

## 本日成果

使用 Arduino Uno、PlatformIO 和 Wokwi，在同一个 loop() 中依次处理三个任务：

- LED 每隔 3000 毫秒切换一次，灭 3 秒、亮 3 秒，完整周期约 6 秒。
- 串口每隔 1000 毫秒打印一次 Tick。
- 随时接收 ? 查询软件保存的 LED 状态；非法字符提示 Unknown command。

本版不使用 delay()。按键保留在电路图中，但本日程序尚未启用按键，也未实现非阻塞按键消抖。
本版没有第六天的 1/0 开关命令。

## 文件与运行

- src/main.cpp：最终的双计时任务与串口查询代码。
- platformio.ini：Arduino Uno / Arduino 框架配置，环境名 uno。
- diagram.json：虚拟电路与接线。
- wokwi.toml：引用 .pio/build/uno/firmware.hex 和 firmware.elf。
- .gitignore：忽略编译产物和编辑器生成配置。

VS Code 打开本目录 → PlatformIO Build → 等待 SUCCESS → Ctrl+Shift+P → Wokwi: Start Simulator。
修改代码后保存并重新 Build，再启动仿真；不要用普通 Run Code 运行 Arduino 程序。
.pio 文件由编译生成，不上传。

## 接线

POWER 5V → 1000Ω 电阻 → LED 阳极 A；阴极 C → D8。
D8 LOW 亮，HIGH 灭。
保留按键 D2 → 1.l、GND → 2.r，同组 2.l 与 2.r 内部连通；本日代码不读取按键。

## 已完成实验

以下结果均由学习者在聊天中确认符合预期：

1. 首版 interval=1000：灯每隔 1 秒切换，串口 ? 可及时查询。
2. interval 改为 3000：灯每隔 3 秒切换，等待期间仍能查询。
3. 最终双计时版本：灯每 3 秒切换、Tick 每 1 秒打印、? 仍能及时回复。

第七天首版曾由助手本地编译成功；最终双计时版由学习者运行并反馈通过。仿真行为反馈与编译记录分开记录。

## 核心知识

### 读取时刻，不暂停等待

millis() 返回程序启动以来经过的毫秒数，时间变量使用 unsigned long。
now - previousMillis >= interval 表示距离上一次执行已经达到规定间隔。
执行后 previousMillis = now 更新记录，不会将 millis() 清零。
若不更新，第一次到时后，条件会持续满足，导致每轮循环都执行任务。

### 不同任务分别计时

LED 使用 previousLedMillis，Tick 使用 previousTickMillis；每个任务只更新自己的记录。
两个独立 if 允许同一轮依次处理多个到时任务，并非真正同时执行。
若第二个改为 else if，第一项满足时本轮会跳过第二项；第二项通常下一轮补做，不等于永久漏掉。
本实现使用 previousMillis = now，适合当前教学；执行和打印耗时可能使实际间隔略有偏差。

### 串口中的回车与换行

'\r' 是回车 CR，ASCII 13；'\n' 是换行 LF，ASCII 10。
command != '\r' && command != '\n' 表示既不是回车也不是换行，才报告非法命令。
Wokwi 串口监视器默认向每行发送内容末尾附加 LF；发送 ? 时可依次收到 '?'、'\n'。
在输入框直接键入反斜杠与 n，通常是两个普通字符，并非实际换行字符。
官方说明：https://docs.wokwi.com/guides/serial-monitor#newline

## 后续学习位置

下一步：把第六天 delay(20) 按键消抖改成 millis() 计时，输入连续稳定至少 20 毫秒后再认可状态。
这部分目前仅介绍了思路，尚未编写或验收，不能算作已完成。

## 来源

最终代码及配置复制自本地 E:\day07_millis；学习记录依据本次聊天整理。
