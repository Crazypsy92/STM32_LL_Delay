/* Includes ------------------------------------------------------------------*/
#include "MyApplication.h"

/* 内部 tick 因子，仅本文件使用 */
static uint32_t g_fac_us = 0;       // 微秒级因子
static uint32_t g_fac_ms = 0;       // 毫秒级因子
volatile uint32_t g_sys_tick = 0;   // 系统滴答计数器，毫秒级 

/* 非阻塞延迟相关变量 */
static uint32_t g_delay_start_tick = 0;
static uint32_t g_delay_target_tick = 0;
static uint8_t  g_delay_in_progress = 0;

/**
  * @brief  初始化 SysTick 延时环境
  * @param  sysclk_MHz: 主频 MHz 数，例：72
  * @retval 无
  * @note   会自动打开 SysTick 中断，1 ms 周期；如用户已自行编写 SysTick_Handler，
  *         请把 g_sys_tick++ 合并进已有中断，否则系统滴答不更新
  */
void LL_delay_init(uint16_t sysclk_MHz)
{
    SysTick->CTRL = 0;                               /* 先关掉 */
    SysTick->CTRL &= ~SysTick_CTRL_CLKSOURCE_Msk;    /* 选择 HCLK/8 */
    g_fac_us = sysclk_MHz / 8;                       /* 1 us 需要多少个 tick */
    
    /* 配置SysTick每1ms产生一次中断 */
    SysTick->LOAD = (uint32_t)g_fac_us * 1000;       /* 1ms对应的tick数 */
    SysTick->VAL  = 0UL;                             /* 清空计数器 */
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;       /* 使能中断 */
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;        /* 启动计数器 */
}

/**
  * @brief  微秒级阻塞延时
  * @param  us: 要延时的微秒数，范围 1~0xFFFFFFFF
  * @retval 无
  * @note   函数内部临时关闭 SysTick 中断，确保精度；中断在返回前自动恢复
  */
void LL_delay_us(uint32_t nus)
{
    uint32_t ticks = nus * g_fac_us;
    uint32_t temp;
    uint32_t old_ctrl = SysTick->CTRL;               /* 保存当前控制寄存器值 */

    /* 关闭SysTick中断，避免干扰阻塞延时 */
    SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
    
    SysTick->LOAD = ticks;
    SysTick->VAL  = 0UL;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;

    do {
        temp = SysTick->CTRL;
    } while ((temp & SysTick_CTRL_ENABLE_Msk) && !(temp & SysTick_CTRL_COUNTFLAG_Msk));

    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    SysTick->VAL   = 0UL;
    
    /* 恢复中断配置 */
    SysTick->CTRL = old_ctrl;
}

/**
  * @brief  毫秒级阻塞延时
  * @param  ms: 要延时的毫秒数，范围 1~65535
  * @retval 无
  * @note   内部循环调用 LL_delay_us(1000)，因此 1 ms 以下分辨率无效
  */
void LL_delay_ms(uint16_t nms)
{
    uint32_t repeat = nms / 1000U;
    uint32_t remain = nms % 1000U;

    while (repeat--) {
        LL_delay_us(1000000UL);
    }
    if (remain) {
        LL_delay_us(remain * 1000UL);
    }
}

/**
  * @brief  启动非阻塞毫秒延时
  * @param  nms: 目标延时毫秒数
  * @retval 无
  * @note   启动后需循环调用 LL_delay_ms_check() 查询完成状态；
  *         不可与阻塞版混用，否则计时基准冲突
  */
void LL_delay_ms_start(uint16_t nms)
{
    g_delay_start_tick = SysTick->VAL;  // 记录当前计数值
    g_delay_target_tick = (uint32_t)nms * g_fac_ms;
    g_delay_in_progress = 1;
}

/**
  * @brief  查询非阻塞延时是否完成
  * @param  无
  * @retval 1: 已完成；0: 未完成
  * @note   必须在 1 ms SysTick 中断正常工作的情况下使用
  */
uint8_t LL_delay_ms_check(void)
{
    if (!g_delay_in_progress) return 1;  // 未启动延时，视为已完成
    
    uint32_t current_tick = g_delay_start_tick - SysTick->VAL;
    if (current_tick >= g_delay_target_tick) {
        g_delay_in_progress = 0;
        return 1;
    }
    return 0;
}

/**
  * @brief  获取系统运行时间（毫秒）
  * @param  无
  * @retval 当前 g_sys_tick 值，溢出后自动回零
  * @note   关中断读取，线程/中断内均可安全调用
  */
uint32_t LL_GetTick(void)
{
    uint32_t tick;
    /* 关闭中断防止读取时发生溢出 */
    __disable_irq();
    tick = g_sys_tick;
    __enable_irq();
    return tick;
}

/**
  * @brief  SysTick 中断服务函数（用户可选）
  * @param  无
  * @retval 无
  * @note   如 MCU 启动文件已弱映射，请取消注释下方代码；
  *         若已在别处实现，请把 g_sys_tick++ 合并进去
  */
// void SysTick_Handler(void)
// {
//     g_sys_tick++;  /* 每毫秒计数+1 */
// }

/* --------------------  printf redirect  -------------------- */
// int fputc(int ch, FILE *f)
// {
//     /* Wait until TXE flag is set */
//     while (!LL_USART_IsActiveFlag_TXE(huart_debug)) { }
//     LL_USART_TransmitData8(huart_debug, (uint8_t)ch);
//     return ch;
// }
/********************************************************
  End Of File
********************************************************/
