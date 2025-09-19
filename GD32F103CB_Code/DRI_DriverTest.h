#ifndef DRI_DriverTest_h
#define DRI_DriverTest_h

#ifdef __cplusplus 
extern "C" { 
#endif 

#include "DRI_ComDriver.h"



/// @brief 驱动测试初始化函数
/// @param  
extern void DRI_DriverTest_Init(void);

/// @brief 驱动测试实时任务
/// @param  
extern void DRI_DriverTest_RealTask(void);


#ifdef __cplusplus 
}
#endif 





#endif
