# 第十二天：EEPROM 保存闪烁间隔

Arduino Uno / PlatformIO / Wokwi，串口 9600。接线沿用第十一天：D8 LOW 亮，D2 INPUT_PULLUP（按下 LOW）。

- interval N：修改 RAM 中正在使用的间隔，范围 100～10000 ms。
- save：将当前间隔写入 EEPROM，从地址 0 开始。保留 on、off、auto、status。
- 启动时：从同一地址读取同一类型的值。若不在合法范围，使用默认值 1000 ms。
- 只保存间隔；启动仍为手动模式且灯灭。程序启动和 interval 命令均不会写入 EEPROM。

EEPROM.get() 读取一个变量大小的数据，EEPROM.put() 按变量大小保存；在 AVR Arduino 库中只更新值变化的字节。
本实验用范围判断识别无效数据，不做掉电写入保护或校验和。

## 验收记录（2026-11-06）

对照实验（第十一天）：改成 500 ms 后重启，学习者确认恢复到 1000 ms。

启动第十二天：Ctrl+Shift+P → Wokwi: Start Simulator；点击 Wokwi Terminal，命令输完按 Enter。

1. 无有效 EEPROM 数据时，代码会提示 Using default interval: 1000 ms（默认分支，本日未单独确认）。
2. interval 500 后发送 save，等待 Saved to EEPROM: 500 ms。
3. 保持仿真运行，点击虚拟 Uno 板上红色 RESET 键，等启动信息再次出现。读回 Loaded from EEPROM: 500 ms，启动为手动模式，灯灭。
4. interval 2000，不发送 save。再按同一个 RESET，仍加载 500 ms。
5. interval 2000 后发送 save，再复位，加载 2000 ms，确认新保存值覆盖旧值。

验证 EEPROM 时优先用同一仿真会话中的板上复位键。停止并新建仿真会话可能丢失模拟存储，不能以此判断真实 Uno 的 EEPROM 是否掉电保留。
步骤 2～5 由学习者在聊天中确认符合预期。PlatformIO 编译成功（RAM 551/2048 字节，Flash 3886/32256 字节）；归档时未重新运行仿真。

## 代码阅读笔记

- intervalAddress 是 EEPROM 内部的起始字节位置，与变量所在的 RAM 地址无关。
- EEPROM.get(intervalAddress, savedInterval) 通过引用将读取结果写入 savedInterval；EEPROM.put 从同一位置写入同类型的数据。Uno 上 unsigned long 为 4 字节，因此本例使用 EEPROM 字节位置 0～3。
- get 只读取，loadInterval 再检查合法范围并选择保存值或默认值。
