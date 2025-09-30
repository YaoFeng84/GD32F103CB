#ifndef DRI_ADC_h
#define DRI_ADC_h

#ifdef __cplusplus 
extern "C" { 
#endif 

#include "DRI_ComDriver.h"

/*------------------------------------------------------------------------------------------------
使用例子：
【1】、初始化ADC
     DRI_ADC_ChannelEnum AdcCh[] = {ADC_CH3_PA3,ADC_CH6_PA6,ADC_CH8_PB0,ADC_CH5_PA5};
     DRI_ADCCnfType adcconfig;
     adcconfig.AdcChNum = sizeof(AdcCh)/sizeof(AdcCh[0]);
     adcconfig.AdcReferenceValue = 3300;
     adcconfig.AdcChp = AdcCh;
     DRI_ADC_Config(&adcconfig);

【2】、读取ADC值
     s32 adcvalue = DRI_ADC0_Read(ADC_CH8_PB0);
------------------------------------------------------------------------------------------------*/

typedef enum
{//值不可修改！！！
     ADC_CH0_PA0 = 0, //ADC通道0
     ADC_CH1_PA1 = 1, //ADC通道1
     ADC_CH2_PA2 = 2, //ADC通道2
     ADC_CH3_PA3 = 3, //ADC通道3
     ADC_CH4_PA4 = 4, //ADC通道4
     ADC_CH5_PA5 = 5, //ADC通道5
     ADC_CH6_PA6 = 6, //ADC通道6
     ADC_CH7_PA7 = 7, //ADC通道7
     ADC_CH8_PB0 = 8, //ADC通道8
     ADC_CH9_PB1 = 9, //ADC通道9
}DRI_ADC_ChannelEnum;//ADC通道枚举

typedef struct
{
     u8 AdcChNum; //ADC通道数量     
     u16 AdcReferenceValue; //ADC参考电压，单位mV
     DRI_ADC_ChannelEnum *AdcChp;  //ADC通道数组指针
}DRI_ADCCnfType;//ADC初始化结构体



/// @brief ADC初始化
/// @param cnf 初始化结构体
/// @return 小于0表示失败，等于0表示成功
extern s8 DRI_ADC_Config(DRI_ADCCnfType *cnf);

/// @brief 读取指定通道的电压值，单位mV
/// @param cn 指定通道
/// @return 小于0:读取失败 大等于0:读取的电压值，单位mV
extern s32 DRI_ADC0_Read(DRI_ADC_ChannelEnum cn);

#ifdef __cplusplus 
}
#endif 

#endif

