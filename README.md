# STM32-PMSM ![](./QPano3/Resources/bt428.svg) ![](./QPano3/Resources/plug.svg)
Project of driving permanent magnet synchronous motor based on STM32F103XXX.

If you think this project has helped you, please give me a star, which is the greatest encouragement to me.

## Tree of source
.  
├── Bin  
├── Control  
├── Display  
├── Doc  
├── Encoder  
├── Feedback  
├── Firmware  
├── Foc  
├── GwDebug  
├── GwOS  
├── Key  
├── Libraries  
├── Math  
├── Pid  
├── Project  
├── Scripts  
├── Statemachine  
├── SVPWM  
├── Task  
├── Test  
├── Usart  
└── User  

|||
|---|---|
|Bin| Tools for change file format |
|Control| Source of control motor |
|Display| Interface of Display |
|Encoder| Driver of Encoder |
|Feedback|Adc for current and get \theta elec |
|Firmware|Build project will create bin file and hex file in this directory|
|Foc| Interface of Foc |


## 简介
基于`stm32`驱动永磁同步电机的项目，`FOC`磁场矢量控制算法，双电阻采样，双闭环PI控制

果你觉得该项目帮助到了你，请给我一颗星星，这是对我最大的鼓励。
