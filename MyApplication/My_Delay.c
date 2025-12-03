/* Includes ------------------------------------------------------------------*/
#include "MyApplication.h"

/* 内部 tick 因子，仅本文件使用 */
static uint32_t g_fac_us = 0;
static uint32_t g_fac_ms = 0;  // 新增：毫秒级因子
volatile uint32_t g_sys_tick = 0;  /* 系统滴答计数器，毫秒级 */

/* 非阻塞延迟相关变量 */
static uint32_t g_delay_start_tick = 0;
static uint32_t g_delay_target_tick = 0;
static uint8_t  g_delay_in_progress = 0;

/* --------------------------------------------------
 * 初始化：SysTick 时钟 = HCLK / 8，同时配置1ms中断
 * ------------------------------------------------*/
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

/* --------------------------------------------------
 * 微秒延时（阻塞）
 * ------------------------------------------------*/
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

/* --------------------------------------------------
 * 毫秒延时（阻塞）
 * ------------------------------------------------*/
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

/* --------------------------------------------------
 * 启动非阻塞毫秒延时
 * ------------------------------------------------*/
void LL_delay_ms_start(uint16_t nms)
{
    g_delay_start_tick = SysTick->VAL;  // 记录当前计数值
    g_delay_target_tick = (uint32_t)nms * g_fac_ms;
    g_delay_in_progress = 1;
}

/* --------------------------------------------------
 * 检查非阻塞延时是否完成
 * 返回值：1-完成，0-未完成
 * ------------------------------------------------*/
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

/* --------------------------------------------------
 * 获取系统滴答值（类似HAL_GetTick）
 * 返回值：系统运行毫秒数（溢出后从0重新计数）
 * ------------------------------------------------*/
uint32_t LL_GetTick(void)
{
    uint32_t tick;
    /* 关闭中断防止读取时发生溢出 */
    __disable_irq();
    tick = g_sys_tick;
    __enable_irq();
    return tick;
}

/* --------------------------------------------------
 * SysTick中断服务函数
 * 每1ms递增一次系统滴答计数器
 * ------------------------------------------------*/
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
