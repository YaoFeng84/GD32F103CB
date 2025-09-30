#ifndef DRI_SPIn_H
#define DRI_SPIn_H

#ifdef __cplusplus 
extern "C" { 
#endif 

#include "DRI_ComDriver.h"

/*------------------------------------------------------------------------------------------------
使用例子：
     DRI_SPICnfType spicnf;
     spicnf.speed = 10000000;          //速度，单位Hz  最终速度会小等于speed的值
     spicnf.WorkMode = SPIMode3;//工作模式
     spicnf.PinSelect = SPI1_SCK_PB13_MISO_PB14_MOSI_PB15;  //SPI引脚选择   
     spicnf.CSport = GPORTB;     //CS端口号
     spicnf.CSpin = GP12;       //CS引脚号
     spicnf.CSDefaultLevel = 1;  //CS默认电平(0:低电平 非0:高电平)
     DRI_SPIn_Config(&spicnf);
------------------------------------------------------------------------------------------------*/

typedef enum
{
     SPI0_SCK_PA5_MISO_PA6_MOSI_PA7 = 0u,
     SPI0_SCK_PB3_MISO_PB4_MOSI_PB5 = 1u,
     SPI1_SCK_PB13_MISO_PB14_MOSI_PB15 = 2u
}DRI_SPI_PinSelectEnm;

typedef enum
{
     SPIMode0 = 0u,
     SPIMode1 = 1u,
     SPIMode2 = 2u,
     SPIMode3 = 3u,
}DRI_SPIWorkMode;//SPI工作模式

typedef struct
{
     u32 speed;          //速度，单位Hz  最终速度会小等于speed的值
     DRI_SPIWorkMode WorkMode;//工作模式
     DRI_SPI_PinSelectEnm PinSelect;  //SPI引脚选择   
     PortNum CSport;     //CS端口号
     PinNum CSpin;       //CS引脚号
     u8 CSDefaultLevel;  //CS默认电平(0:低电平 非0:高电平)
}DRI_SPICnfType;


/***************************************************************************
* 函 数 名: DRI_SPIn_Config
* 功能描述：SPIn配置函数
* 入口参数：
            DRI_SPICnfType *cnfp：配置结构体指针
* 出口参数：
            -1:表示配置失败
            0:表示配置成功
* 注意事项: 

* 例     如:
* 修改记录 :
*           2025-09-29 BY:
***************************************************************************/
extern s8 DRI_SPIn_Config(DRI_SPICnfType *cnfp);

/***************************************************************************
* 函 数 名: DRI_SPI0_ReadWriteByte
* 功能描述：阻塞型 SPI0数据收发函数
* 入口参数：
            u8 u8data：待发送的字节数据
* 出口参数：
            收到的字节数据
* 注意事项: 

* 例     如:
* 修改记录 :
*           2025-09-29 BY:
***************************************************************************/
extern u8 DRI_SPI0_ReadWriteByte(u8 u8data);

/***************************************************************************
* 函 数 名: DRI_SPI0_CS
* 功能描述：SPI0片选引脚输出控制函数
* 入口参数：
            u8 cs：0:片选引脚输出低电平(0:低电平 非0:高电平)
                 非0:片选引脚输出高电平(1:高电平 非1:低电平)
* 出口参数：
            无
* 注意事项: 

* 例     如:
* 修改记录 :
*           2025-09-29 BY:
***************************************************************************/
extern void DRI_SPI0_CS(u8 cs);

/***************************************************************************
* 函 数 名: DRI_SPI1_ReadWriteByte
* 功能描述：阻塞型 SPI1数据收发函数
* 入口参数：
            u8 u8data：待发送的字节数据
* 出口参数：
            收到的字节数据
* 注意事项: 

* 例     如:
* 修改记录 :
*           2025-09-29 BY:
***************************************************************************/
extern u8 DRI_SPI1_ReadWriteByte(u8 u8data);

/***************************************************************************
* 函 数 名: DRI_SPI1_CS
* 功能描述：SPI1片选引脚输出控制函数
* 入口参数：
            u8 cs：0:片选引脚输出低电平(0:低电平 非0:高电平)
                 非0:片选引脚输出高电平(1:高电平 非1:低电平)
* 出口参数：
            无
* 注意事项: 

* 例     如:
* 修改记录 :
*           2025-09-29 BY:
***************************************************************************/
extern void DRI_SPI1_CS(u8 cs);



#ifdef __cplusplus 
}
#endif 


#endif
