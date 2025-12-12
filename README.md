# STM32F103 微秒/毫秒延时库（LL 库，阻塞 + 非阻塞）

## 简介

本仓库提供一套**不占用额外定时器**的高精度延时方案，适用于 STM32F103 全系列。\
利用 **SysTick 时钟（HCLK/8）** 实现微秒级、毫秒级延时，同时支持**阻塞**与**非阻塞**两种编程模型，方便在裸机或中断环境内灵活使用。\
代码已使用 **Keil5 + STM32F103C8T6** 验证，1 µs 分辨率误差 < ±1 µs\@72 MHz。

## 功能特点

*   **微秒延时**：`LL_delay_us()`，最小单位 1 µs，阻塞式
*   **毫秒延时**：`LL_delay_ms()`，最大 65 535 ms，内部自动拆分成 µs
*   **非阻塞延时**：`LL_delay_ms_start()` + `LL_delay_ms_check()`，不卡死 CPU(这是类似RTOS的绝对延时，一般非阻塞用LL\_GetTick就够了
*   **系统滴答**：`LL_GetTick()`，获取从上电开始的毫秒计数（溢出自动回零）
*   **零依赖**：仅使用 SysTick，不占用 TIMx，兼容任何 LL/HAL 混合工程
*   **时钟自适应**：调用 `LL_delay_init(sysclk_MHz)` 填入主频即可，换晶振不改库

## 快速上手

1.  把下面 2 个文件加入你的工程：

My\_Delay.h&#x20;

My\_Delay.c

    2. 在 `main.c` 里初始化（72 MHz 示例）：

    LL_delay_init(72);          // 告诉库当前主频

1.  想用就调：

        LL_delay_us(10);            // 阻塞 10 µs
        LL_delay_ms(500);           // 阻塞 500 ms
2.  非阻塞闪烁 LED：

        LL_delay_ms_start(1000);    // 启动 1 s 定时
        while (1) {
            if (LL_delay_ms_check()) {
                LED_TOGGLE;         // 电平翻转
                LL_delay_ms_start(1000); // 重新定时
            }
            /* 其余任务可放这里 */
        }

## 移植说明

| 步骤                    | 要点                                                               |
| :-------------------- | :--------------------------------------------------------------- |
| 1. 提供 SysTick 1 ms 中断 | 把 `SysTick_Handler()` 中的 `g_sys_tick++` 放到it文件的SysTick\_Handler里 |
| 2. 修改主频               | 更换晶振后，只需改 `LL_delay_init(48)` 或 `LL_delay_init(56)` 等            |
| 3. 与其他延时共存            | 本库临时关闭中断的时长 < 1 µs，可与 LL\_mDelay / OS 延时并存，但**不建议在中断内调用阻塞版**     |

## API 速查

```c
void LL_delay_init(uint16_t sysclk_MHz); // 初始化
void LL_delay_us(uint32_t us);           // 阻塞 µs
void LL_delay_ms(uint16_t ms);           // 阻塞 ms
void LL_delay_ms_start(uint16_t nms);    // 启动非阻塞
uint8_t LL_delay_ms_check(void);         // 查询非阻塞完成
uint32_t LL_GetTick(void);               // 读取系统滴答
```

## 常见问题

**Q1: 最大能延时多久？**

*   `LL_delay_us()`：理论 2^32-1 µs ≈ 71 min
*   `LL_delay_ms()`：最大 65535 ms（16 bit 参数）

**Q2: 中断被关掉会影响系统吗？**

*   仅在 `LL_delay_us()` 内部关闭 可屏蔽中断，不可屏蔽中断照常响应；关闭时长 < 1 µs，对通信/控制无感

**Q3: 我想在 FreeRTOS 里用？**

*   建议改用 RTOS 自带 API,rtos有类似gettick的东西

## 许可证

MIT License，随意商用，保留原作者信息即可。
