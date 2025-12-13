/**
 * @file MyApplication.h
 * @brief 应用程序总头文件
 * @details 包含系统基础头文件、标准库头文件及自定义模块头文件，
 *          作为应用程序的统一头文件入口
 * @author 西伯利亚小菜狗
 * @date 2025-12-05
 * @version 1.0
 */
#ifndef __MyApplication_H__
#define __MyApplication_H__

/* Includes ------------------------------------------------------------------*/
#include "main.h"

#include <stdio.h>
#include "stdlib.h"
#include <stdint.h>

#include "My_Delay.h"

#define DEBUG 0
#if DEBUG
// 启用调试时，定义DEBUG_PRINTF为printf
#define DEBUG_PRINTF(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
// 禁用调试时，定义为空宏（不产生任何代码）
#define DEBUG_PRINTF(fmt, ...) do {} while(0)
#endif

#endif
/********************************************************
  End Of File
********************************************************/
