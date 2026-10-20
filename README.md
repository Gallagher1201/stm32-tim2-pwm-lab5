# STM32F103 TIM2 定时与 PWM 实验

实验五：TIM2 更新中断控制 PA0 每秒翻转，完整周期 2 秒；TIM2_CH1 配置 1 kHz PWM，以 2 秒周期改变占空比。

打开 [Keil 工程](project/MDK-ARM/Lab5_TIM2.uvprojx)，选择 Blink 或 Breathing 编译目标。两个目标均通过 ARM Compiler 5.06 编译（0 错误、0 警告）。仿真方法和参数见 [工程说明](project/README.md)。

- [实验报告 Markdown](report/实验报告.md)
- [实验报告 PDF](report/实验报告.pdf)
- [博文离线草稿](report/博文草稿.md)
- [实验截图](picture)
- [完整提交包](Lab5_Submission.zip)
- [CSDN 草稿编辑入口](https://editor.csdn.net/md?articleId=168544976)（需作者登录，未公开发布）

本次仅软件仿真，未上板。Blink 的 PA0 翻转、Breathing 的占空比包络已观察到。当前 Keil 未显示 PA0 复用 PWM 跳变，`pwm_reference` 仅为 CNT/CCR 比较参考，不是引脚实测。报告已明确记录验证边界。
