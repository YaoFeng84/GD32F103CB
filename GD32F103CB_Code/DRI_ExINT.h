#ifndef DRI_ExINT_h
#define DRI_ExINT_h

#ifdef __cplusplus 
extern "C" { 
#endif 

#include "DRI_ComDriver.h"

/*------------------------------------------------------------------------------------------------	
特点：
     【1】、支持16个外部中断通道
     【2】、上升沿、下降沿、双边沿 这三种触发
     【3】、引脚号（pinn）相同时以最后一次为主，之前无效。
     
使用例子：PA3引脚的外部中断设置
     ExIntCnfType exintconfig;
     
     exintconfig.cbfp = exint3;
     exintconfig.pinn = GP03;
     exintconfig.portn = GPORTA;
     exintconfig.pri = 15;
     exintconfig.tm = ITFallEdge;
     DRI_ExINT_Config(&exintconfig);


     void exint3(void)
     {
          DRI_GPIO_OUT_Turn(GPORTB,GP09);
     }
------------------------------------------------------------------------------------------------*/

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


/***************************************************************************
* 函 数 名: DRI_ExINT_Config
* 功能描述：外部引脚中断配置函数
* 入口参数：
            ExIntCnfType *eict：配置结构体指针
* 出口参数：
            -1:表示配置失败
            0:表示配置成功
* 注意事项:  
            PA2,PB2,PC2,PD2,PE2引脚序号相同的不能同时出现。
* 例     如:
* 修改记录 :
*           2025-09-29 BY:
***************************************************************************/
extern s8 DRI_ExINT_Config(ExIntCnfType *eict);


#ifdef __cplusplus 
}
#endif 

#endif
