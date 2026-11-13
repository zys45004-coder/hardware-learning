# 第十八天：观察 I2C 波形

Uno R3 / PlatformIO / Wokwi。LCD 接 5V、GND、A4/SDA、A5/SCL，地址 0x27。
逻辑分析仪 D0 接 SDA，D1 接 SCL，GND 共地。它只观察总线。
本实验不初始化或更新 LCD，只用 Wire 发一次地址探测。

1. Ctrl+Shift+P → Wokwi: Start Simulator。启动应显示 0x27: ACK。
2. 点终端按小写 s，可额外探测一次；字符不回显。
3. 停止仿真。Wokwi 应将 i2c-capture.vcd 保存到工程根目录。
4. 波形中寻找 START，随后是 7 位地址（0100111）和 1 位写标志（0），组成 0x4E。第九个 SCL 脉冲期间，SDA 为低表示 ACK。最后是 STOP。
5. 可选对照：停止仿真后断开 LCD 的 SDA（保留分析仪的 SDA），重启后预期显示 NACK，第九拍 SDA 为高。结束后恢复接线。

参考：https://docs.wokwi.com/guides/logic-analyzer
## 验收记录

PlatformIO 编译成功（RAM 404/2048 字节，Flash 3788/32256 字节）。下面两份 VCD 是学习者实际录制的 Wokwi 仿真波形。

| 录波 | 接线 | 前 8 拍 | 第 9 拍 SDA |
| --- | --- | --- | --- |
| i2c-ack.vcd | LCD SDA 正常连接 | 01001110 | LOW，ACK |
| i2c-nack.vcd | 仅断开 LCD SDA | 01001110 | HIGH，NACK |

两次都是对 7 位地址 0x27 的写方向探测。前 8 拍合起来显示为 0x4E，包含地址和写入位；没有发送数据字节。第 9 拍由应答方决定 SDA 是否被拉低。分析仪在断线对照时仍接在 Uno 一侧。

提交前已恢复 LCD SDA → A4，并核对分析仪 D0 仍接 A4、D1 仍接 A5。i2c-capture.vcd 是每次录制会覆盖的临时输出，两份保留录波使用独立文件名。
