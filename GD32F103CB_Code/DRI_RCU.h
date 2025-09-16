#ifndef DRI_RCU_h
#define DRI_RCU_h

#ifdef __cplusplus 
extern "C" { 
#endif 

#include "DRI_ComDriver.h"

typedef enum
{
     LP_Rst    = 0u,   //深度睡眠/待机复位发生时的复位
     WWDGT_Rst = 1u,   //窗口看门狗定时器复位发生时的复位
     FWDGT_Rst = 2u,   //独立看门狗复位发生时的复位
     SW_Rst    = 3u,   //软件复位发生时的复位
     POR_Rst   = 4u,   //电源复位发生时的复位
     EP_Rst    = 5u,   //外部引脚复位发生时的复位
     Multiply_Rst = 6u,   //多源复位发生时的复位
}ResetFlag;

extern ResetFlag DRI_RCU_GetResetFlag(void);
extern void DRI_RCU_ClearResetFlag(void);



#ifdef __cplusplus 
}
#endif 

#endif
