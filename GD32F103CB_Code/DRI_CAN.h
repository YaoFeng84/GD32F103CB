#ifndef DRI_CAN_h
#define DRI_CAN_h

#ifdef __cplusplus 
extern "C" { 
#endif 

#include "DRI_ComDriver.h"

typedef struct 
{     
     u32 FrameID;//帧ID
     u8 FrameType;//帧类型(0:标准帧 非0:扩展帧)
     u8 FrameDataLen;//数据长度
     u8 FrameData[8];//数据
}DRI_CAN_CommunicatDataType;//通信数据类型

typedef enum
{
     CAN0_RX_PA11_TX_PA12 = 0,
     CAN0_RX_PB8_TX_PB9
}DRI_CAN_PinSelectEnm;//CAN引脚选择

typedef void(*DRI_CAN_ReceCBFType)(DRI_CAN_CommunicatDataType*);//接收回调函数类型

typedef struct 
{
     u32 Bps;//波特率
     u8 IntPri;//中断优先级
     DRI_CAN_PinSelectEnm PinSelect;//引脚选择
     DRI_CAN_ReceCBFType ReceCBF;//接收回调函数
}DRI_CANCnfType;//CAN配置类型

/***************************************************************************
* 函 数 名: DRI_CANx_Config
* 功能描述：CANx配置函数
* 入口参数：
            DRI_CANCnfType *cnf:CAN配置参数
* 出口参数：
          小于0:表示配置失败
              0:表示配置成功
* 注意事项:  

* 例     如:
* 修改记录 :
          
*           2025-09-24 BY:
***************************************************************************/
extern s8 DRI_CANx_Config(DRI_CANCnfType *cnf);

/***************************************************************************
* 函 数 名: DRI_CAN0_SendData
* 功能描述：CAN0阻塞型发送数据函数
* 入口参数：
          DRI_CAN_CommunicatDataType *data:待发送的数据
* 出口参数：
          小于0:表示发送失败
             0:表示发送成功
* 注意事项:  

* 例     如:
* 修改记录 :
          
*           2025-09-24 BY:
***************************************************************************/
extern s8 DRI_CAN0_SendData(DRI_CAN_CommunicatDataType *data);


#ifdef __cplusplus 
}
#endif 

#endif

