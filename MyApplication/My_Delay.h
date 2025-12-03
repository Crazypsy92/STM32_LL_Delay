#ifndef __MY_DELAY_H_
#define __MY_DELAY_H_

#include "MyApplication.h"

#define huart_debug USART2

/* ---------- 对外接口 ---------- */
void LL_delay_init(uint16_t sysclk_MHz);   /* 初始化 */
void LL_delay_us(uint32_t  us);            /* 微秒级阻塞延时 */
void LL_delay_ms(uint16_t  ms);            /* 毫秒级阻塞延时 */
void LL_delay_ms_start(uint16_t nms);
uint8_t LL_delay_ms_check(void);
uint32_t LL_GetTick(void);                 /* 获取系统滴答值（毫秒） */

extern volatile uint32_t g_sys_tick;

#endif /* __MY_DELAY_H_ */
