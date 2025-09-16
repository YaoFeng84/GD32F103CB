#ifndef DRI_ComDriver_h
#define DRI_ComDriver_h

#include "TypeMro.h"
#include "gd32f10x.h"
//
#include "system_gd32f10x.h"
//外设头文件
#include "gd32f10x_libopt.h"
//


//USB设备外设
//USB内核
#include "usbd_core.h"
//#include "drv_usb_dev.h"
#include "usbd_lld_regs.h"
#include "usbd_lld_int.h"
//USB设备内核
#include "usbd_core.h"
#include "usbd_enum.h"


extern void NULLFP(void);

//返回固件版本号
extern u8* DRI_ComDriver_OutVersion(void);

//返回芯片Flash空间大小，单位：字节
extern u32 DRI_ComDriver_OutFlashSize(void);

//返回芯片RAM空间大小，单位：字节
extern u32 DRI_ComDriver_OutRAMSize(void);

//uid1:(0~31)、uid2:(32~63)、uid3:(64~95)共96位
extern void DRI_ComDriver_OutUID(u32 *uid1, u32 *uid2, u32 *uid3);

extern void DRI_ComDriver_Delay1ms(void);
extern void DRI_ComDriver_Delay1us(void);

typedef void(*DRI_ComDriver_DoubleVoidFuncType)(void);//定义一个双void的函数指针类型

/***************************************************************************
* 函 数 名: DRI_ComDriver_SetHardFaultIntterrupt_FCB
* 功能描述：设置硬件错误中断回调 函数
* 入口参数：
            DRI_ComDriver_DoubleVoidFuncType fcbp：硬件错误中断回调函数指针
* 出口参数：
            无
* 注意事项:  
            
* 例     如:
* 修改记录 :
*           2025-09-08 BY:YJX
***************************************************************************/
extern void DRI_ComDriver_SetHardFaultIntterrupt_FCB(DRI_ComDriver_DoubleVoidFuncType fcbp);

/***************************************************************************
* 函 数 名: DRI_ComDriver_SetSP
* 功能描述：设置栈顶指针 函数
* 入口参数：
            u32 _sp:栈顶指针值
* 出口参数：
            无
* 注意事项:  
            修改栈顶指针
* 例     如:
* 修改记录 :
*           2025-09-08 BY:YJX
***************************************************************************/
extern void DRI_ComDriver_SetSP(u32 _sp);

/*-------------------------------------------------
函数名:DRI_ComDriver_DisableAllPeripheralInterrupt
功   能:关闭所有外设的各自中断
参   数:
       无
返回值:
       无
注   意:
       无
示   例:
作   者:YJX
版   本:V1.0
时   间:2025-09-08
-------------------------------------------------*/
extern void DRI_ComDriver_DisableAllPeripheralInterrupt(void);

/*-------------------------------------------------
函数名:DRI_ComDriver_ResetAllPeripheral
功   能:复位所有外设
参   数:
       无
返回值:
       无
注   意:
       无
示   例:
作   者:YJX
版   本:V1.0
时   间:2025-09-08
-------------------------------------------------*/
extern void DRI_ComDriver_ResetAllPeripheral(void);

/***************************************************************************
* 函 数 名: DRI_ComDriver_EnableAllINT
* 功能描述：使能外设总中断 函数
* 入口参数：
            无
* 出口参数：
            无
* 注意事项:  
            无
* 例     如:
* 修改记录 :
*           2025-09-08 BY:YJX
***************************************************************************/
extern void DRI_ComDriver_EnableAllINT(void);

/***************************************************************************
* 函 数 名: DRI_ComDriver_DisableAllINT
* 功能描述：禁能外设总中断 函数
* 入口参数：
            无
* 出口参数：
            无
* 注意事项:  
            无
* 例     如:
* 修改记录 :
*           2025-09-08 BY:YJX
***************************************************************************/
extern void DRI_ComDriver_DisableAllINT(void);

/***************************************************************************
* 函 数 名: DRI_ComDriver_EnableAllFault
* 功能描述：使能内核总异常 函数
* 入口参数：
            无
* 出口参数：
            无
* 注意事项:  
            无
* 例     如:
* 修改记录 :
*           2025-09-08 BY:YJX
***************************************************************************/
extern void DRI_ComDriver_EnableAllFault(void);

/***************************************************************************
* 函 数 名: DRI_ComDriver_DisableAllFault
* 功能描述：禁能内核总异常 函数
* 入口参数：
            无
* 出口参数：
            无
* 注意事项:  
            无
* 例     如:
* 修改记录 :
*           2025-09-08 BY:YJX
***************************************************************************/
extern void DRI_ComDriver_DisableAllFault(void);

/***************************************************************************
* 函 数 名: DRI_ComDriver_CoreReset
* 功能描述：内核复位 函数
* 入口参数：
            无
* 出口参数：
            无
* 注意事项:  
            无
* 例     如:
* 修改记录 :
*           2025-09-08 BY:YJX
***************************************************************************/
extern void DRI_ComDriver_CoreReset(void);

/***************************************************************************
* 函 数 名: DRI_ComDriver_SysReset
* 功能描述：外设复位 函数
* 入口参数：
            无
* 出口参数：
            无
* 注意事项:  
            无
* 例     如:
* 修改记录 :
*           2025-09-08 BY:YJX
***************************************************************************/
extern void DRI_ComDriver_SysReset(void);

typedef enum
{
     GPORTA = 0u,
     GPORTB = 1u,
     GPORTC = 2u
}PortNum;

typedef enum
{
     GP00 = 0u,
     GP01 = 1u,
     GP02 = 2u,
     GP03 = 3u,
     GP04 = 4u,
     GP05 = 5u,
     GP06 = 6u,
     GP07 = 7u,
     GP08 = 8u,
     GP09 = 9u,
     GP10 = 10u,
     GP11 = 11u,
     GP12 = 12u,
     GP13 = 13u,
     GP14 = 14u,
     GP15 = 15u
}PinNum;



typedef enum
{
     GDOut = 0u,//数字输出
     GDIn = 1u  //数字输入
}PinMode;

typedef enum
{
     ITFallEdge = 0u,//下降沿
     ITRiseEdge = 1u,//上升沿
     ITBothEdg = 2u //双边沿
}INTTriggerMode;//触发模式枚举

typedef struct
{
     PortNum portn;      //外部中断端口号
     PinNum pinn;        //外部中断引脚号
     INTTriggerMode tm;  //触发模式
     u8 pri;             //中断优先级(0~15，越小优先级越高)
     void (*cbfp)(void); //中断回调函数指针
}ExIntCnfType;





#endif

