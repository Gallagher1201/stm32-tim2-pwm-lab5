# STM32F103 TIM2 定时与 PWM 实验

用 PA0（TIM2_CH1）完成 2 秒亮灭和 2 秒呼吸灯。仅进行 Keil 软件仿真，未连接开发板。

## 打开与运行

打开 `MDK-ARM/Lab5_TIM2.uvprojx`，选择 `Blink` 或 `Breathing`，按 F7 编译，Ctrl+F5 进入仿真。工程使用 ARM Compiler 5.06 update 5、STM32F103C8 和 Keil STM32F1xx_DFP 2.2.0。本机 Keil 位于 `E:\keli5\UV4\UV4.exe`。

Debug 选择 Use Simulator，模拟器 DLL 为 SARMCM3.DLL，参数 `-REMAP`，设备 DLL 为 DARMSTM.DLL，参数 `-pSTM32F103C8`。初始化脚本为 `Simulation.ini`。

命令窗口依次执行 `LOG > result.log`、`lab_check()`、`g`。脚本运行约 4.51 秒仿真时间后暂停，再执行 `LOG OFF`。脚本只读取状态，不产生 PWM，不改写 PA0。

## 参数

HSE=8 MHz，PLL×9，SYSCLK=72 MHz，APB1=36 MHz，TIM2 时钟=72 MHz。

|目标|PSC|ARR|行为|
|---|---:|---:|---|
|Blink|7199|9999|每秒更新中断，PA0 每秒翻转，完整周期 2 秒|
|Breathing|71|999|PWM 1 kHz，每 4 ms 更新 CCR1，500 步完成 2 秒呼吸|

PWM 使用向上计数、PWM1、高电平有效。占空比为 CCR1/(ARR+1)，CCR1=0 和 1000 分别对应 0% 和 100%。平方曲线用于细化暗部的占空比变化。

## 波形与验证边界

Logic Analyzer 添加 `(PORTA & 1)`、`duty_counts`、`pwm_reference`。duty_counts 范围设为 0～1000，pwm_reference 范围设为 0～1。长时间窗口观察呼吸包络，毫秒窗口观察比较参考脉冲。

本机仿真观察到 Blink 的 PA0 翻转和 Breathing 的 CCR1 周期变化。PWM 模式下 PORTA 未显示复用输出跳变，不能据此宣称 PA0 引脚 PWM 已测量通过。`pwm_reference` 是固件读取 CNT 与 CCR1 后形成的比较参考，不是 GPIO 回读，也不是上板测量。主循环中的比较采样只用于辅助观察，PWM 本身由 TIM2 外设配置产生；采样会有指令执行和中断带来的时序误差，CCR1 预装载更新附近也不等同于内部影子寄存器的瞬时值。

## 文件

`Core/Src/main.c` 为应用代码；`Drivers/CMSIS` 为芯片与内核头文件；`MDK-ARM` 包含工程、启动文件、构建输出和仿真日志。未使用 CubeMX 生成项目，因此没有伪造的 `.ioc` 文件。

硬件连接仅作设计说明：PA0 经约 330 Ω 电阻连接 LED 阳极，阴极接 GND。PC13 板载 LED 不是 TIM2_CH1 输出引脚。STM32F103C8 实际具有 TIM2、TIM3、TIM4，不能直接选用 TIM5。

参考：
- https://blog.csdn.net/Morzart/article/details/134123197
- https://blog.csdn.net/Xkccsdn147/article/details/134191212
- https://blog.csdn.net/weixin_52345779/article/details/135313203

CMSIS/ST 文件保留原始版权与许可声明；应用代码用于本次课程实验。
