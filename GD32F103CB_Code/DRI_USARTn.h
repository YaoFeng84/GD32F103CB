#ifndef DRI_USARTn_h
#define DRI_USARTn_h

#ifdef __cplusplus 
extern "C" { 
#endif 

#include "DRI_ComDriver.h"

/*------------------------------------------------------------------------------------------------
特点：
     【1】、优先级值越小，越高。


使用例子：
     UsartCnfType usartconfig;

     usartconfig.bps = 115200;
     usartconfig.PinSelect = USART2_RX_PB11_TX_PB10;
     usartconfig.DataBit = DataBits8;
     usartconfig.Parity = ParityNone;
     usartconfig.StopBit = OneStopBit;
     usartconfig.rhwfc.FEnable = 0;//
     usartconfig.IntPri = 10;
     usartconfig.RecData_IntEnable = 1;
     usartconfig.Recfp = usar2;
     usartconfig.SendOK_IntEnable = 0;
     usartconfig.SendINTDefaultState = 0;
     usartconfig.SendOKfp = 0;
     DRI_USARTn_Config(&usartconfig);
 

     void usar2(u8 rd)
     {
          DRI_USART2_SendByte(rd);
     }
------------------------------------------------------------------------------------------------*/

typedef enum
{
     DataBits8 = 0u,//8位数据宽度
     DataBits9 = 1u,//9位数据宽度
}DataBitType;

typedef enum
{
     HalfStopBit = 0u,//半位停止位
     OneStopBit = 1u,//1位停止位
     OneAndHalfStopBit = 2u,//1.5位停止位
     TwoStopBit = 3u,//2位停止位
}StopBitType;

typedef enum
{
     ParityNone = 0u,//无校验
     ParityEven = 1u,//偶校验
     ParityOdd = 2u,//奇校验
}ParityType;

typedef enum
{
     USART0_RX_PA10_TX_PA9 = 1,
     USART0_RX_PB7_TX_PB6,    //USART0有两组引脚，只能二选一！！
     USART1_RX_PA3_TX_PA2,
     USART2_RX_PB11_TX_PB10
}DRI_UART_PinSelectEnm;//UART引脚选择

typedef struct
{
     u8 FEnable;              //接收硬流控使能位(0:禁用 非0:使用)
     u8 RHWFlowXonLevel;      //接收硬流控XON电平(0:低电平 非0:高电平)
     u8 RHWFlowDefaultLevel;  //接收硬流控默认电平(0:低电平 非0:高电平)
     PortNum RFlowcontrolp;   //接收硬流控端口号
     PinNum RFlowcontroln;    //接收硬流控引脚号
}RHWFlowCrl;

typedef struct
{
//---常规配置
     u32 bps;            //波特率(2400~921600)
     DataBitType DataBit;//数据位
     StopBitType StopBit;//停止位
     ParityType  Parity; //校验模式
     RHWFlowCrl rhwfc;   //接收硬流控
//---中断配置
     u8 IntPri;        //串口中断优先级(0~15，越小优先级越高，超过范围禁止中断)
//---引脚选择
     DRI_UART_PinSelectEnm PinSelect;//引脚选择
//---接收部分
     void (*Recfp)(u8);  //接收到数据回调函数指针
     u8 RecData_IntEnable;//接收数据中断使能(0:禁止 非0:使能)
//---发送部分
     void (*SendOKfp)(void);//发送完成回调函数指针
     u8 SendOK_IntEnable;//发送完成中断使能(0:禁止 非0:使能)
     u8 SendINTDefaultState;//发送中断默认状态(0:关闭 1:开启)
}DRI_USARTCnfType;

/***************************************************************************
* 函 数 名: DRI_USARTn_Config
* 功能描述：USART配置函数
* 入口参数：
            DRI_USARTCnfType *cnfp：配置结构体指针
* 出口参数：
            -1:表示配置失败
            0:表示配置成功
* 注意事项: 

* 例     如:
* 修改记录 :
*           2025-09-26 BY:
***************************************************************************/
extern s8 DRI_USARTn_Config(DRI_USARTCnfType *cnfp);

/***************************************************************************
* 函 数 名: DRI_USART0_SendByte
* 功能描述：USART0 阻塞型 发送1个字节函数
* 入口参数：
            u8 sd：待发送字节
* 出口参数：
            无
* 注意事项:  
            无
* 例     如:
* 修改记录 :
*           2025-09-26 BY:
***************************************************************************/
extern void DRI_USART0_SendByte(u8 sd);

/***************************************************************************
* 函 数 名: DRI_USART1_SendByte
* 功能描述：USART1 阻塞型 发送1个字节函数
* 入口参数：
            u8 sd：待发送字节
* 出口参数：
            无
* 注意事项:  
            无
* 例     如:
* 修改记录 :
*           2025-09-26 BY:
***************************************************************************/
extern void DRI_USART1_SendByte(u8 sd);

/***************************************************************************
* 函 数 名: DRI_USART2_SendByte
* 功能描述：USART2 阻塞型 发送1个字节函数
* 入口参数：
            u8 sd：待发送字节
* 出口参数：
            无
* 注意事项:  
            无
* 例     如:
* 修改记录 :
*           2025-09-26 BY:
***************************************************************************/
extern void DRI_USART2_SendByte(u8 sd);



#ifdef __cplusplus 
}
#endif 

#endif

