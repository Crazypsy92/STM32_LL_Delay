/**
 * @file My_Delay.h
 * @brief 延时功能头文件
 * @details 声明基于SysTick的延时函数、系统滴答相关函数及外部变量
 * @author 西伯利亚小菜狗
 * @date 2025-12-05
 * @version 1.0
 */
#ifndef __MY_DELAY_H_
#define __MY_DELAY_H_

#include "MyApplication.h"

/**
 * @brief 调试串口定义
 * @details 指定printf重定向使用的USART外设
 */
#define huart_debug USART2

/* ---------- 对外接口 ---------- */

/**
 * @brief 初始化SysTick延时环境
 * @param sysclk_MHz - 系统主频，单位MHz
 * @retval 无
 */
void LL_delay_init(uint16_t sysclk_MHz);

/**
 * @brief 微秒级阻塞延时
 * @param us - 延时微秒数
 * @retval 无
 */
void LL_delay_us(uint32_t  us);

/**
 * @brief 毫秒级阻塞延时
 * @param ms - 延时毫秒数
 * @retval 无
 */
void LL_delay_ms(uint16_t  ms);

/**
 * @brief 启动非阻塞毫秒延时
 * @param nms - 目标延时毫秒数
 * @retval 无
 */
void LL_delay_ms_start(uint16_t nms);

/**
 * @brief 查询非阻塞延时是否完成
 * @param 无
 * @retval 1-已完成，0-未完成
 */
uint8_t LL_delay_ms_check(void);

/**
 * @brief 获取系统运行时间（毫秒）
 * @param 无
 * @retval 当前系统滴答值
 */
uint32_t LL_GetTick(void);

/**
 * @brief 系统滴答计数器
 * @details 毫秒级计数器，由SysTick中断每1ms递增一次
 */
extern volatile uint32_t g_sys_tick;

#endif /* __MY_DELAY_H_ */
