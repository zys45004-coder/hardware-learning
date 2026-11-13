# 第十七天：I2C 地址扫描

Arduino Uno / PlatformIO / Wokwi，串口 9600。沿用第十六天接线，LCD 的 SDA 接 A4，SCL 接 A5，VCC 接 5V，GND 接公共地。本程序只扫描总线，不更新 LCD，也不运行夜灯控制。光敏模块的 AO 接 A0，不参与 I2C。

## 扫描原理

- Wire.begin() 初始化 Uno 的硬件 I2C。
- 扫描 7 位地址 0x08～0x77，跳过保留地址。
- beginTransmission(address) 选定要探测的地址；endTransmission() 实际发出地址。本实验不发送数据内容。
- 返回 0 表示有应答，返回 2 表示地址没有应答；其他错误单独报告。总线超时会结束本轮扫描。
- 扫描只列出应答地址，不会识别设备型号。

## 运行与验收记录

Ctrl+Shift+P → Wokwi: Start Simulator。查看 Wokwi Terminal：预计启动时发现 0x27，设备数 1。终端发送小写 s 可以再扫描一次；程序不会回显输入。

后续对照：停止仿真，临时断开 LCD 的 SDA 或移除 LCD 后重新启动，预计设备数为 0；完成后恢复原接线。

本地 PlatformIO 编译成功（RAM 404/2048 字节，Flash 4212/32256 字节）。学习者确认启动和输入 s 再扫都能找到 0x27，恢复 SDA 后再次启动也符合预期。断线时的 Devices found: 0 是对照实验的预期，未单独收到结果。归档前核对了 diagram.json 中 lcd1:SDA → uno:A4 已恢复。

补充：Serial.println(address, HEX) 只改变数字的显示进制；0x 前缀由前面的 Serial.print 输出。Wire.setWireTimeout(25000, true) 设置 25 ms 等待超时，发生超时时重置 Uno 的 I2C 硬件；break 退出本轮地址循环，仍会打印汇总。