# 第十三天：PWM 调节 LED 亮度

Arduino Uno / PlatformIO / Wokwi，串口 9600。LED 阴极从 D8 改接 D9，其他接线保持不变：5V → 1000Ω → LED 阳极；按键接 D2 和 GND（INPUT_PULLUP）。

D9 LOW 时灯亮，HIGH 时灯灭。灯亮时，brightness N 设定低电平所占的比例；analogWrite 参数表示高电平比例，因此需要反向换算。灯灭时 D9 保持 HIGH。以 25% 为例，输出参数约为 191，高电平约占 75%，低电平约占 25%。比例描述亮灯时间，不等于肉眼感受到的亮度比例。

- brightness N：接受 1～100 的整数，默认 100。灯亮时立即调光；灯灭时保持灭，下次亮起时使用新设定。关闭使用 off。
- status：显示模式、亮灭、切换间隔和设定亮度。
- on / off / auto / interval N：保留原功能。自动模式在亮起的阶段使用设定亮度；PWM 不需要 loop 手动翻转 D9。
- save：沿用第十二天，仅保存闪烁间隔。亮度复位后恢复 100%。
- 非法亮度参数报错，保留现有设置。

## 运行与验收记录

PlatformIO Build → SUCCESS → Ctrl+Shift+P → Wokwi: Start Simulator。点击 Wokwi Terminal，逐条输入命令并按 Enter。

1. on：以默认 100% 点亮。
2. brightness 25：LED 变暗，status 显示亮度设置 25%。
3. brightness 100：恢复全亮。
4. off 后设置 brightness 25：灯保持灭；再 on，以较低亮度点亮。
5. interval 1000 后发送 auto：每秒切换，亮起阶段使用 25%，熄灭阶段完全熄灭。按键接管后回到手动模式，亮度设定仍为 25%。
6. brightness 0、brightness 101、brightness abc 均报错；status 确认设定仍为 25%。

学习者确认 25% 与 100% 亮度有区别，开关与亮度独立、自动闪烁、按键接管和非法亮度参数检查均符合预期。PlatformIO 编译成功（RAM 647/2048 字节，Flash 4438/32256 字节）；归档时未重新运行仿真。

## 代码阅读笔记

- applyLedOutput() 按 ledOn 选择输出；灯灭时直接输出 HIGH，改亮度不会开灯。
- 灯开启时，将设定百分比乘以 255 再除以 100，整数除法前加 50 实现四舍五入；最后用 255 减去该刻度得到 analogWrite 参数。
- PWM 调整每个快速周期的低电平占比；interval 调整自动模式的亮灭切换间隔。
