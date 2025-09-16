#ifndef DRI_GPIO_h
#define DRI_GPIO_h

#ifdef __cplusplus 
extern "C" { 
#endif 

#include "DRI_ComDriver.h"


/***************************************************************************
* 函 数 名: DRI_GPIO_Config
* 功能描述：GPIO引脚配置函数
* 入口参数：
            PortNum enPort：端口号(取GPORTA,GPORTB,GPORTC)
            PinNum u16Pin：引脚号(取GP00~GP15)
            PinMode gm：GDOut表示数字输出 GDIn表示数字输入
* 出口参数：
            -1:表示配置失败
            0:表示配置成功
* 注意事项:  
            无
* 例     如:
* 修改记录 :
*           2025-08-29 BY:YJX
***************************************************************************/
extern s8 DRI_GPIO_Config(PortNum portn, PinNum pinn, PinMode gm);

/***************************************************************************
* 函 数 名: DRI_GPIO_OUT_1
* 功能描述：GPIO引脚输出高电平函数
* 入口参数：
            PortNum portn：端口号(取GPORTA,GPORTB,GPORTC,GPORTD,GPORTE,GPORTH)
            PinNum pinn：引脚号(取GP00~GP15)
* 出口参数：
            无
* 注意事项:  
            无
* 例     如:
* 修改记录 :
*           2025-08-29 BY:YJX
***************************************************************************/
extern void DRI_GPIO_OUT_1(PortNum portn, PinNum pinn);

/***************************************************************************
* 函 数 名: DRI_GPIO_OUT_0
* 功能描述：GPIO引脚输出低电平函数
* 入口参数：
            PortNum enPort：端口号(取GPORTA,GPORTB,GPORTC,GPORTD,GPORTE,GPORTH)
            PinNum u16Pin：引脚号(取GP00~GP15)
* 出口参数：
            无
* 注意事项:  
            无
* 例     如:
* 修改记录 :
*           2025-08-29 BY:YJX
***************************************************************************/
extern void DRI_GPIO_OUT_0(PortNum portn, PinNum pinn);

/***************************************************************************
* 函 数 名: DRI_GPIO_OUT_Turn
* 功能描述：GPIO引脚输出翻转函数
* 入口参数：
            PortNum enPort：端口号(取GPORTA,GPORTB,GPORTC,GPORTD,GPORTE,GPORTH)
            PinNum u16Pin：引脚号(取GP00~GP15)
* 出口参数：
            无
* 注意事项:  
            无
* 例     如:
* 修改记录 :
*           2025-08-29 BY:YJX
***************************************************************************/
extern void DRI_GPIO_OUT_Turn(PortNum portn, PinNum pinn);

/***************************************************************************
* 函 数 名: DRI_GPIO_Read
* 功能描述：GPIO引脚读函数
* 入口参数：
            PortNum enPort：端口号(取GPORTA,GPORTB,GPORTC,GPORTD,GPORTE,GPORTH)
            PinNum u16Pin：引脚号(取GP00~GP15)
* 出口参数：
            0:表示低电平
          非0:表示高电平
* 注意事项:  
            无
* 例     如:
* 修改记录 :
*           2025-08-29 BY:YJX
***************************************************************************/
extern u8 DRI_GPIO_Read(PortNum portn, PinNum pinn);


#ifdef __cplusplus 
}
#endif 

#endif

