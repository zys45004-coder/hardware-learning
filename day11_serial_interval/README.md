# 第十一天：串口设置闪烁间隔

Arduino Uno / PlatformIO / Wokwi，串口 9600。
源自第十天工程，保留输入回显、退格、CR/LF/CRLF 行结束、超长行丢弃、非阻塞消抖与按键接管。
接线：5V → 1000Ω → LED A，LED C → D8（LOW 亮、HIGH 灭）；D2 接按键另一端接 GND，INPUT_PULLUP（按下 LOW）。

## 命令

- on / off：手动模式，开灯 / 关灯。
- auto：自动模式，从当前亮灭起等待一个完整间隔，然后定时切换。
- status：显示模式、亮灭、切换间隔。
- interval 500：设置自动切换间隔为 500 ms。命令与参数之间使用一个空格。

间隔初始为 1000 ms，允许 100～10000 ms（含边界）。仅接受非空十进制数字串；正负号、小数、尾随文字、额外空格均不接受。前导零允许。
设置成功只改变间隔并重新计时，不改变模式或当前亮灭；手动模式仍需 auto 才会自动闪烁。
无效参数不改变间隔、模式或计时起点；自动模式原有的定时切换仍继续。
32 字符数组最多接收 31 个内容字符，超长整行拒绝。设置只保存在内存，重启恢复 1000 ms。
切换间隔为亮或灭的一段时间；完整亮灭周期约为间隔的两倍。

## 运行

VS Code 打开本目录，PlatformIO Build，等待 SUCCESS。
Ctrl+Shift+P → Wokwi: Start Simulator，点击 Wokwi Terminal，输入英文命令并按 Enter。

## Wokwi 验收记录

1. 启动灯灭，status 返回 MANUAL / OFF / Interval: 1000 ms。
2. interval 2000 成功；保持手动模式且灯灭；status 显示 2000 ms。
3. auto：每隔 2 秒切换，完整周期约 4 秒。
4. interval 500：当前亮灭不变，从此刻起约 500 ms 后首次切换，之后每隔 500 ms 切换。
5. interval 50：Invalid interval，status 仍是 500 ms。
6. interval 500abc、interval、interval 10001 均拒绝，status 确认保留最后一次有效设置。
7. interval 100 和 interval 10000 两个边界均接受。
8. 设为 500 ms 后，on 保持亮，auto 恢复闪烁；长按只切换一次并回到手动模式，off 关灯，status 仍显示 500 ms。

## 学习进度

已理解：4000 毫秒等于 4 秒；const 去掉后变量可在运行时修改；无效参数应报错并保留原值。
本日新增：strncmp 识别前缀、指针跳过前缀、逐位将字符数字转换为数值、验证通过后才更新配置。
PlatformIO 编译 SUCCESS（RAM 495/2048 字节，Flash 3548/32256 字节）。以上仿真结果由学习者在聊天中确认；归档时未重新启动仿真。回显、退格、空行和超长行恢复沿用第十天，本日未单独复测。

## 代码阅读笔记

- strncmp(command, "interval ", 9) 检查命令开头的 8 个字母和 1 个空格，command + 9 指向参数的第一个字符。
- parseInterval 逐位执行 value = value * 10 + (text[i] - '0')，例如 "527" 依次得到 5、52、527。
- 解析返回 true 表示有效，引用参数带回数值；失败时不修改间隔或计时起点。
- 单独输入 interval 时给出缺少有效参数的提示；未知命令仍提示 Unknown command。
