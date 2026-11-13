# 第十九天：I2C 总线上的 LCD 与 RTC

Uno R3 / PlatformIO / Wokwi。LCD1602 的地址为 0x27，DS1307 实时时钟的地址为 0x68。
两个模块均接 5V、GND，并联共用 Uno 的 A4/SDA、A5/SCL。DS1307 的 SQW 不接。
本实验只验证共用总线与读时间，不包含夜灯判断。

## 运行与验收

1. Ctrl+Shift+P → Wokwi: Start Simulator。终端应找到 0x27、0x68，设备数为 2。
2. LCD 第一行显示 YYYY-MM-DD，第二行显示 hh:mm:ss；观察秒数变化。终端也打印同一份时间。
3. Wokwi 的 DS1307 设置 initTime=now；代码只读取 RTC，不在每次 Uno 启动时写时间。验收看秒数是否变化，不要求与电脑显示完全一致。
4. millis() 只负责约每秒刷新，真正的日期和时间来自 rtc.now()。
## 已确认的仿真结果

- 正常接线：串口扫到 0x27 和 0x68；LCD 显示日期和时间，秒数正常变化。
- 断线对照：仅断开 rtc1:SDA → uno:A4，LCD 仍可显示 RTC not found。断线时的扫描数量未单独记录。
- 恢复 RTC SDA 后重新启动：两个地址和时钟显示均恢复。提交前核对了两个模块的 SDA 都接 A4、SCL 都接 A5。

PlatformIO 编译成功（RAM 481/2048 字节，Flash 8446/32256 字节）。

## 代码阅读笔记

- 两个对象共用 Wire：lcd 的地址在构造时指定，DS1307 的 0x68 地址由库提供。
- rtc.now() 读取一份日期和时间快照；日期与时间都从这份快照格式化，避免跨午夜时两行不一致。
- isValid() 检查日期与时间是否合法，不代表已经校准。toString() 直接改写字符数组中的占位符。
- rtc.begin() 失败时，return 只退出 setup()。loop() 仍运行，但 rtcReady 保持 false，所以不刷新时间。

参考：https://docs.wokwi.com/parts/wokwi-ds1307
