#ifndef DRI_Systick_h
#define DRI_Systick_h

#ifdef __cplusplus 
extern "C" { 
#endif 

#include "DRI_ComDriver.h"

extern s8 DRI_SysTick_Config(u32 msn,u8 priority,void (*cbfp)(void));
extern void DRI_SysTick_INT_DISABLE(void);
extern void DRI_SysTick_Disable(void);
extern void DRI_SysTick_INTFLAG_CLR(void);
extern void DRI_SysTick_INT_EXIT(void);

#ifdef __cplusplus 
}
#endif 

#endif

