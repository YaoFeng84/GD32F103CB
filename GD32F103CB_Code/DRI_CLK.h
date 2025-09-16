#ifndef DRI_CLK_h
#define DRI_CLK_h

#ifdef __cplusplus 
extern "C" { 
#endif 

#include "DRI_ComDriver.h"


typedef struct
{
     u32 SysTickFreq;     ///< SysTick嘀嗒时钟频率
     u32 hclkFreq;       ///< Hclk 频率.(数据手册中最高108MHz)
     u32 pclk1Freq;      ///< Pclk1 频率.(数据手册中最高54MHz)
     u32 pclk2Freq;      ///< Pclk2 频率.(数据手册中最高108MHz)
}ClkFreqValue;

extern s8 DRI_CLK_Config(void);
extern void DRI_CLK_GetClockFreq(ClkFreqValue *pstcClkFreq);

#ifdef __cplusplus 
}
#endif 

#endif

