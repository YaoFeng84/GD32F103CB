/********************************************************************************************************************************************
*                                                                                                                                           *
*              ---------------------------------以下是模块的修改记录区-----------------------------------------                             *
*                                                                                                                                           *
********************************************************************************************************************************************/
/**********************************************
 * 内容：
 * 日期：2025-08-29
 * 作者：YJX
 * 版本号：V1.0（初版）
 ***********************************************
 * 修改内容：
 * 修改日期：
 * 修改作者：
 * 版本号：
 ***********************************************
*/
/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下模块的说明区-----------------------------------------                                  *
*                                                                                                                                           *
********************************************************************************************************************************************/
/*	
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
*/
#include "DRI_USARTn.h"
/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下模块的对接函数区-----------------------------------------                                  *
*                                                                                                                                           *
********************************************************************************************************************************************/
     
/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的变量申明和宏定义区--------------------------------                      *
*                                                                                                                                           *
********************************************************************************************************************************************/
extern s8 DRI_GPIO_Config(PortNum portn, PinNum pinn, PinMode gm);
extern void DRI_GPIO_OUT_1(PortNum portn, PinNum pinn);
extern void DRI_GPIO_OUT_0(PortNum portn, PinNum pinn);

typedef void(*USARTReceEventCBF)(u8);//定义USART接收事件函数指针类型
typedef void(*USARTSendEventCBF)(void);//定义USART发送事件函数指针类型

#define XON    0x11
#define XOFF   0x13

static USARTReceEventCBF ReceFunP0 = (USARTReceEventCBF)NULLFP;
static USARTReceEventCBF ReceFunP1 = (USARTReceEventCBF)NULLFP;
static USARTReceEventCBF ReceFunP2 = (USARTReceEventCBF)NULLFP;
//
static USARTSendEventCBF SendOKFunP0 = NULLFP;
static USARTSendEventCBF SendOKFunP1 = NULLFP;
static USARTSendEventCBF SendOKFunP2 = NULLFP;
//
static RHWFlowCrl RHWFC0,RHWFC1,RHWFC2;//,RHWFC3,RHWFC4,RHWFC5,RHWFC6,RHWFC7;//接收硬流件
/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的内部函数申明区------------------------------------                          *
*                                                                                                                                           *
********************************************************************************************************************************************/
static s8 USARTn_Config(u32 usart_periph,DRI_USARTCnfType *cnfp);

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的系统函数代码区------------------------------------                          *
*                                                                                                                                           *
********************************************************************************************************************************************/
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
s8 DRI_USARTn_Config(DRI_USARTCnfType *cnfp)
{
     s8 s8result;
     u32 usartn;
     IRQn_Type usart_irq;
     RHWFlowCrl *Rhwfcp;
     rcu_periph_enum usart_rcu;

     switch(cnfp->PinSelect)
     {          
          case USART0_RX_PA10_TX_PA9:
               usart_irq = USART0_IRQn;
               usart_rcu = RCU_USART0;
               usartn = USART0;
               Rhwfcp = &RHWFC0;
               //
               //使能GPIOA时钟
               rcu_periph_clock_enable(RCU_GPIOA);
               gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);//PA9_Tx
               gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_10);//PA10_Rx
               //                              
               break;

          case USART0_RX_PB7_TX_PB6:
               usart_irq = USART0_IRQn;
               usart_rcu = RCU_USART0;
               usartn = USART0;
               Rhwfcp = &RHWFC0;
               //
               //使能GPIOB时钟
               rcu_periph_clock_enable(RCU_GPIOB);
               gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6);//PB6_Tx
               gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_7);//PB7_Rx
               //                              
               break;

          case USART1_RX_PA3_TX_PA2:
               usart_irq = USART1_IRQn;
               usart_rcu = RCU_USART1;
               usartn = USART1;
               Rhwfcp = &RHWFC1;
               //
               //使能GPIOA时钟
               rcu_periph_clock_enable(RCU_GPIOA);              
               gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2);//PA2_TX
               gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_3);//PA3_RX
               //
               break;

          case USART2_RX_PB11_TX_PB10:
               usart_irq = USART2_IRQn;
               usart_rcu = RCU_USART2;
               usartn = USART2;
               Rhwfcp = &RHWFC2;
               //               
               //使能GPIOB时钟
               rcu_periph_clock_enable(RCU_GPIOB);              
               gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_10);//PB10_TX
               gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_11);//PB11_RX
               //
               break;

          default:
               return -1;
     }    

     NVIC_DisableIRQ(usart_irq);//禁止中断，防止配置过程中产生中断

     //接收硬流控
     Rhwfcp->FEnable = cnfp->rhwfc.FEnable;
     Rhwfcp->RFlowcontroln = cnfp->rhwfc.RFlowcontroln;
     Rhwfcp->RFlowcontrolp = cnfp->rhwfc.RFlowcontrolp;
     Rhwfcp->RHWFlowDefaultLevel = cnfp->rhwfc.RHWFlowDefaultLevel;
     Rhwfcp->RHWFlowXonLevel = cnfp->rhwfc.RHWFlowXonLevel;
     //接收硬流控引脚 配置
     if(Rhwfcp->FEnable)
     {//使能硬流控
          //接收硬流控引脚配置为输出
          DRI_GPIO_Config(Rhwfcp->RFlowcontrolp,Rhwfcp->RFlowcontroln,GDOut);
          //配置默认流控电平
          if(Rhwfcp->RHWFlowDefaultLevel)
          {
               DRI_GPIO_OUT_1(Rhwfcp->RFlowcontrolp,Rhwfcp->RFlowcontroln);
          }
          else
          {
               DRI_GPIO_OUT_0(Rhwfcp->RFlowcontrolp,Rhwfcp->RFlowcontroln);
          }  
     }

     //使能USART时钟
     rcu_periph_clock_enable(usart_rcu);
     

     //串口模块配置
     s8result = USARTn_Config(usartn,cnfp);
     //
     if((cnfp->IntPri < 16) && (s8result == 0))
     {//允许中断
          NVIC_SetPriority(usart_irq, cnfp->IntPri);//设置中断优先级
          NVIC_ClearPendingIRQ(usart_irq);//清除中断标志
          NVIC_EnableIRQ(usart_irq);//使能中断
          //nvic_irq_enable(usart_irq, 2U, 0U);
     }
          
     return s8result;
}



void USART0_IRQHandler(void)
{
     u8 u8temp;
     u32 uartn = USART0;
     USARTReceEventCBF receFP = ReceFunP0;
     USARTSendEventCBF sendFP = SendOKFunP0;

     if(RESET != usart_flag_get(uartn, USART_FLAG_RBNE))
     {
          if(RESET != usart_interrupt_flag_get(uartn, USART_INT_FLAG_RBNE))
          {
               u8temp = usart_data_receive(uartn);//读取接收数据后会自动清除标志位(usart_data_receive(uartn) & 0x7F);
               receFP(u8temp);//
          }
          else
          {
               u8temp = usart_data_receive(uartn);//读取接收数据后会自动清除标志位(usart_data_receive(uartn) & 0x7F);
          }
     }
     
     if(RESET != usart_flag_get(uartn, USART_FLAG_TC))
     {
          usart_flag_clear(uartn, USART_FLAG_TC);
          //发送中断
          if(RESET != usart_interrupt_flag_get(uartn, USART_INT_FLAG_TC))
          {
               sendFP();//处理发送中断
          }
     }

     //溢出错误
     if(RESET != usart_flag_get(uartn, USART_FLAG_ORERR))
     {
          usart_flag_clear(uartn, USART_FLAG_ORERR);
          u8temp = usart_data_receive(uartn);//读掉错误数据          
     }

     //噪声错误
     if(RESET != usart_flag_get(uartn, USART_FLAG_NERR))
     {
          usart_flag_clear(uartn, USART_FLAG_NERR);
          u8temp = usart_data_receive(uartn);//读掉错误数据          
     }

     //帧错误
     if(RESET != usart_flag_get(uartn, USART_FLAG_FERR))
     {
          usart_flag_clear(uartn, USART_FLAG_FERR);
          u8temp = usart_data_receive(uartn);//读掉错误数据          
     }

     //奇偶错误
     if(RESET != usart_flag_get(uartn, USART_FLAG_PERR))
     {
          usart_flag_clear(uartn, USART_FLAG_PERR);
          u8temp = usart_data_receive(uartn);//读掉错误数据          
     }
}//

void USART1_IRQHandler(void)
{
     u8 u8temp;
     u32 uartn = USART1;
     USARTReceEventCBF receFP = ReceFunP1;
     USARTSendEventCBF sendFP = SendOKFunP1;

     if(RESET != usart_flag_get(uartn, USART_FLAG_RBNE))
     {
          if(RESET != usart_interrupt_flag_get(uartn, USART_INT_FLAG_RBNE))
          {
               u8temp = usart_data_receive(uartn);//读取接收数据后会自动清除标志位(usart_data_receive(uartn) & 0x7F);
               receFP(u8temp);//
          }
          else
          {
               u8temp = usart_data_receive(uartn);//读取接收数据后会自动清除标志位(usart_data_receive(uartn) & 0x7F);
          }
     }
     
     if(RESET != usart_flag_get(uartn, USART_FLAG_TC))
     {
          usart_flag_clear(uartn, USART_FLAG_TC);
          //发送中断
          if(RESET != usart_interrupt_flag_get(uartn, USART_INT_FLAG_TC))
          {
               sendFP();//处理发送中断
          }
     }

     //溢出错误
     if(RESET != usart_flag_get(uartn, USART_FLAG_ORERR))
     {
          usart_flag_clear(uartn, USART_FLAG_ORERR);
          u8temp = usart_data_receive(uartn);//读掉错误数据          
     }

     //噪声错误
     if(RESET != usart_flag_get(uartn, USART_FLAG_NERR))
     {
          usart_flag_clear(uartn, USART_FLAG_NERR);
          u8temp = usart_data_receive(uartn);//读掉错误数据          
     }

     //帧错误
     if(RESET != usart_flag_get(uartn, USART_FLAG_FERR))
     {
          usart_flag_clear(uartn, USART_FLAG_FERR);
          u8temp = usart_data_receive(uartn);//读掉错误数据          
     }

     //奇偶错误
     if(RESET != usart_flag_get(uartn, USART_FLAG_PERR))
     {
          usart_flag_clear(uartn, USART_FLAG_PERR);
          u8temp = usart_data_receive(uartn);//读掉错误数据          
     }
}//

void USART2_IRQHandler(void)
{
     u8 u8temp;
     u32 uartn = USART2;
     USARTReceEventCBF receFP = ReceFunP2;
     USARTSendEventCBF sendFP = SendOKFunP2;

     if(RESET != usart_flag_get(uartn, USART_FLAG_RBNE))
     {
          if(RESET != usart_interrupt_flag_get(uartn, USART_INT_FLAG_RBNE))
          {
               u8temp = usart_data_receive(uartn);//读取接收数据后会自动清除标志位(usart_data_receive(uartn) & 0x7F);
               receFP(u8temp);//
          }
          else
          {
               u8temp = usart_data_receive(uartn);//读取接收数据后会自动清除标志位(usart_data_receive(uartn) & 0x7F);
          }
     }
     
     if(RESET != usart_flag_get(uartn, USART_FLAG_TC))
     {
          usart_flag_clear(uartn, USART_FLAG_TC);
          //发送中断
          if(RESET != usart_interrupt_flag_get(uartn, USART_INT_FLAG_TC))
          {
               sendFP();//处理发送中断
          }
     }

     //溢出错误
     if(RESET != usart_flag_get(uartn, USART_FLAG_ORERR))
     {
          usart_flag_clear(uartn, USART_FLAG_ORERR);
          u8temp = usart_data_receive(uartn);//读掉错误数据          
     }

     //噪声错误
     if(RESET != usart_flag_get(uartn, USART_FLAG_NERR))
     {
          usart_flag_clear(uartn, USART_FLAG_NERR);
          u8temp = usart_data_receive(uartn);//读掉错误数据          
     }

     //帧错误
     if(RESET != usart_flag_get(uartn, USART_FLAG_FERR))
     {
          usart_flag_clear(uartn, USART_FLAG_FERR);
          u8temp = usart_data_receive(uartn);//读掉错误数据          
     }

     //奇偶错误
     if(RESET != usart_flag_get(uartn, USART_FLAG_PERR))
     {
          usart_flag_clear(uartn, USART_FLAG_PERR);
          u8temp = usart_data_receive(uartn);//读掉错误数据          
     }
}//

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的用户函数代码区------------------------------------                          *
*                                                                                                                                           *
********************************************************************************************************************************************/
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
void DRI_USART0_SendByte(u8 sd)
{
     while (RESET == usart_flag_get(USART0, USART_FLAG_TBE));
     usart_data_transmit(USART0, sd);
}

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
void DRI_USART1_SendByte(u8 sd)
{
     while (RESET == usart_flag_get(USART1, USART_FLAG_TBE));
     usart_data_transmit(USART1, sd);
}

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
void DRI_USART2_SendByte(u8 sd)
{
     while (RESET == usart_flag_get(USART2, USART_FLAG_TBE));
     usart_data_transmit(USART2, sd);
}

// /***************************************************************************
// * 函 数 名: DRI_USART1_NonBlockSendByte
// * 功能描述：USART1 非阻塞型 发送1个字节函数
// * 入口参数：
//             u8 sd：待发送字节
// * 出口参数：
//             无
// * 注意事项:  
//             配合发送完成中断使用
// * 例     如:
// * 修改记录 :
// *           2022-06-23 BY:YJX
// ***************************************************************************/
// void DRI_USART1_NonBlockSendByte(u8 sd)
// {
//      M4_USART1->DR_f.TDR = sd;//写数据到发送缓存
// }


// /***************************************************************************
// * 函 数 名: DRI_USART2_NonBlockSendByte
// * 功能描述：USART2 非阻塞型 发送1个字节函数
// * 入口参数：
//             u8 sd：待发送字节
// * 出口参数：
//             无
// * 注意事项:  
//             配合发送完成中断使用
// * 例     如:
// * 修改记录 :
// *           2022-06-23 BY:YJX
// ***************************************************************************/
// void DRI_USART2_NonBlockSendByte(u8 sd)
// {
//      M4_USART2->DR_f.TDR = sd;//写数据到发送缓存
// }


// /***************************************************************************
// * 函 数 名: DRI_USART3_NonBlockSendByte
// * 功能描述：USART3 非阻塞型 发送1个字节函数
// * 入口参数：
//             u8 sd：待发送字节
// * 出口参数：
//             无
// * 注意事项:  
//             配合发送完成中断使用
// * 例     如:
// * 修改记录 :
// *           2022-06-23 BY:YJX
// ***************************************************************************/
// void DRI_USART3_NonBlockSendByte(u8 sd)
// {
//      M4_USART3->DR_f.TDR = sd;//写数据到发送缓存
// }


// /***************************************************************************
// * 函 数 名: DRI_USART1_EnableReceINT
// * 功能描述：USART1 使能接收中断 函数
// * 入口参数：
//             无
// * 出口参数：
//             无
// * 注意事项:  
            
// * 例     如:
// * 修改记录 :
// *           2022-06-23 BY:YJX
// ***************************************************************************/
// void DRI_USART1_EnableReceINT(void)
// {
//      M4_USART_TypeDef *Usartx = M4_USART1;
//      u16 u16temp = Usartx->DR_f.RDR;//清除原缓存数据
//      u16temp = u16temp;
//      USART_ClearStatus(Usartx, UsartFrameErr);//清除帧错误
//      USART_ClearStatus(Usartx, UsartParityErr);//清除校验错误
//      USART_ClearStatus(Usartx, UsartOverrunErr);//清除 
//      USART_FuncCmd(Usartx, UsartRxInt, Enable);//使能接收数据和接收错误中断
// }

// /***************************************************************************
// * 函 数 名: DRI_USART2_EnableReceINT
// * 功能描述：USART2 使能接收中断 函数
// * 入口参数：
//             无
// * 出口参数：
//             无
// * 注意事项:  
            
// * 例     如:
// * 修改记录 :
// *           2022-06-23 BY:YJX
// ***************************************************************************/
// void DRI_USART2_EnableReceINT(void)
// {
//      M4_USART_TypeDef *Usartx = M4_USART2;
//      u16 u16temp = Usartx->DR_f.RDR;//清除原缓存数据
//      u16temp = u16temp;
//      USART_ClearStatus(Usartx, UsartFrameErr);//清除帧错误
//      USART_ClearStatus(Usartx, UsartParityErr);//清除校验错误
//      USART_ClearStatus(Usartx, UsartOverrunErr);//清除 
//      USART_FuncCmd(Usartx, UsartRxInt, Enable);//使能接收数据和接收错误中断
// }


// /***************************************************************************
// * 函 数 名: DRI_USART1_DisableReceINT
// * 功能描述：USART1 禁止接收中断 函数
// * 入口参数：
//             无
// * 出口参数：
//             无
// * 注意事项:  
            
// * 例     如:
// * 修改记录 :
// *           2022-06-23 BY:YJX
// ***************************************************************************/
// void DRI_USART1_DisableReceINT(void)
// {
//      USART_FuncCmd(M4_USART1, UsartRxInt, Disable);//禁止接收数据和接收错误中断
// }

// /***************************************************************************
// * 函 数 名: DRI_USART2_DisableReceINT
// * 功能描述：USART2 禁止接收中断 函数
// * 入口参数：
//             无
// * 出口参数：
//             无
// * 注意事项:  
            
// * 例     如:
// * 修改记录 :
// *           2022-06-23 BY:YJX
// ***************************************************************************/
// void DRI_USART2_DisableReceINT(void)
// {
//      USART_FuncCmd(M4_USART2, UsartRxInt, Disable);//禁止接收数据和接收错误中断
// }

// /***************************************************************************
// * 函 数 名: DRI_USART1_EnableSendOKINT
// * 功能描述：USART1 使能发送完成中断 函数
// * 入口参数：
//             无
// * 出口参数：
//             无
// * 注意事项:  
            
// * 例     如:
// * 修改记录 :
// *           2022-06-23 BY:YJX
// ***************************************************************************/
// void DRI_USART1_EnableSendOKINT(void)
// {
//      USART_FuncCmd(M4_USART1, UsartTxCmpltInt, Enable);//使能发送完成中断
// }

// /***************************************************************************
// * 函 数 名: DRI_USART2_EnableSendOKINT
// * 功能描述：USART2 使能发送完成中断 函数
// * 入口参数：
//             无
// * 出口参数：
//             无
// * 注意事项:  
            
// * 例     如:
// * 修改记录 :
// *           2022-06-23 BY:YJX
// ***************************************************************************/
// void DRI_USART2_EnableSendOKINT(void)
// {
//      USART_FuncCmd(M4_USART2, UsartTxCmpltInt, Enable);//使能发送完成中断
// }

// /***************************************************************************
// * 函 数 名: DRI_USART1_DisableSendOKINT
// * 功能描述：USART1 禁止发送完成中断 函数
// * 入口参数：
//             无
// * 出口参数：
//             无
// * 注意事项:  
            
// * 例     如:
// * 修改记录 :
// *           2022-06-23 BY:YJX
// ***************************************************************************/
// void DRI_USART1_DisableSendOKINT(void)
// {
//      USART_FuncCmd(M4_USART1, UsartTxCmpltInt, Disable);//禁止发送完成中断
// }

// /***************************************************************************
// * 函 数 名: DRI_USART2_DisableSendOKINT
// * 功能描述：USART2 禁止发送完成中断 函数
// * 入口参数：
//             无
// * 出口参数：
//             无
// * 注意事项:  
            
// * 例     如:
// * 修改记录 :
// *           2022-06-23 BY:YJX
// ***************************************************************************/
// void DRI_USART2_DisableSendOKINT(void)
// {
//      USART_FuncCmd(M4_USART2, UsartTxCmpltInt, Disable);//禁止发送完成中断
// }

// /***************************************************************************
// * 函 数 名: DRI_USART1_XON
// * 功能描述：USART1 阻塞型 发送软流控XON数据 函数
// * 入口参数：
//             无
// * 出口参数：
//             无
// * 注意事项:  
            
// * 例     如:
// * 修改记录 :
// *           2022-06-23 BY:YJX
// ***************************************************************************/
// void DRI_USART1_XON(void)
// {
//      DRI_USART1_SendByte(XON);
// }

// /***************************************************************************
// * 函 数 名: DRI_USART2_XON
// * 功能描述：USART2 阻塞型 发送软流控XON数据 函数
// * 入口参数：
//             无
// * 出口参数：
//             无
// * 注意事项:  
            
// * 例     如:
// * 修改记录 :
// *           2022-06-23 BY:YJX
// ***************************************************************************/
// void DRI_USART2_XON(void)
// {
//      DRI_USART2_SendByte(XON);
// }

// /***************************************************************************
// * 函 数 名: DRI_USART1_XOFF
// * 功能描述：USART1 阻塞型 发送软流控XOFF数据 函数
// * 入口参数：
//             无
// * 出口参数：
//             无
// * 注意事项:  
            
// * 例     如:
// * 修改记录 :
// *           2022-06-23 BY:YJX
// ***************************************************************************/
// void DRI_USART1_XOFF(void)
// {
//      DRI_USART1_SendByte(XOFF);
// }

// /***************************************************************************
// * 函 数 名: DRI_USART2_XOFF
// * 功能描述：USART2 阻塞型 发送软流控XOFF数据 函数
// * 入口参数：
//             无
// * 出口参数：
//             无
// * 注意事项:  
            
// * 例     如:
// * 修改记录 :
// *           2022-06-23 BY:YJX
// ***************************************************************************/
// void DRI_USART2_XOFF(void)
// {
//      DRI_USART2_SendByte(XOFF);
// }

// /***************************************************************************
// * 函 数 名: DRI_USART1_HW_Flowcontrol_ON
// * 功能描述：USART1 控制接收硬流控输出 XON电平 函数
// * 入口参数：
//             无
// * 出口参数：
//             无
// * 注意事项:  
//             接收硬流控配置时需要使能
// * 例     如:
// * 修改记录 :
// *           2022-06-23 BY:YJX
// ***************************************************************************/
// void DRI_USART1_HW_Flowcontrol_ON(void)
// {
//      USARTn_HW_Flowcontrol_ON(&RHWFC1);
// }

// /***************************************************************************
// * 函 数 名: DRI_USART2_HW_Flowcontrol_ON
// * 功能描述：USART2 控制接收硬流控输出 XON电平 函数
// * 入口参数：
//             无
// * 出口参数：
//             无
// * 注意事项:  
//             接收硬流控配置时需要使能
// * 例     如:
// * 修改记录 :
// *           2022-06-23 BY:YJX
// ***************************************************************************/
// void DRI_USART2_HW_Flowcontrol_ON(void)
// {
//      USARTn_HW_Flowcontrol_ON(&RHWFC2);
// }

// /***************************************************************************
// * 函 数 名: DRI_USART1_HW_Flowcontrol_OFF
// * 功能描述：USART1 控制接收硬流控输出 XOFF电平 函数
// * 入口参数：
//             无
// * 出口参数：
//             无
// * 注意事项:  
//             接收硬流控配置时需要使能
// * 例     如:
// * 修改记录 :
// *           2022-06-23 BY:YJX
// ***************************************************************************/
// void DRI_USART1_HW_Flowcontrol_OFF(void)
// {
//      USARTn_HW_Flowcontrol_OFF(&RHWFC1);
// }

// /***************************************************************************
// * 函 数 名: DRI_USART2_HW_Flowcontrol_OFF
// * 功能描述：USART2 控制接收硬流控输出 XOFF电平 函数
// * 入口参数：
//             无
// * 出口参数：
//             无
// * 注意事项:  
//             接收硬流控配置时需要使能
// * 例     如:
// * 修改记录 :
// *           2022-06-23 BY:YJX
// ***************************************************************************/
// void DRI_USART2_HW_Flowcontrol_OFF(void)
// {
//      USARTn_HW_Flowcontrol_OFF(&RHWFC2);
// }

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的内部函数代码区------------------------------------                          *
*                                                                                                                                           *
********************************************************************************************************************************************/
static s8 USARTn_Config(u32 usart_periph,DRI_USARTCnfType *cnfp)
{
     /* USART configure */
     usart_deinit(usart_periph);//关闭USART
     //配置波特率
     usart_baudrate_set(usart_periph,cnfp->bps);//设置波特率
     //配置校验位
     if(cnfp->Parity == ParityNone)
     {
         usart_parity_config(usart_periph,USART_PM_NONE);//无校验
     }
     else if(cnfp->Parity == ParityOdd)
     {
         usart_parity_config(usart_periph,USART_PM_ODD);//奇校验
     }
     else if(cnfp->Parity == ParityEven)
     {
         usart_parity_config(usart_periph,USART_PM_EVEN);//偶校验
     }
     else
     {
         return -1;
     }
     //配置数据位
     if(cnfp->DataBit == DataBits8)
     {
         usart_word_length_set(usart_periph,USART_WL_8BIT);//8位数据位
     }
     else if(cnfp->DataBit == DataBits9)
     {
         usart_word_length_set(usart_periph,USART_WL_9BIT);//9位数据位
     }
     else
     {
         return -1;
     }
     //配置停止位
     if(cnfp->StopBit == HalfStopBit)
     {
         usart_stop_bit_set(usart_periph,USART_STB_0_5BIT);//0.5位停止位
     }
     else if(cnfp->StopBit == OneStopBit)
     {
         usart_stop_bit_set(usart_periph,USART_STB_1BIT);//1位停止位
     }
     else if(cnfp->StopBit == OneAndHalfStopBit)
     {
         usart_stop_bit_set(usart_periph,USART_STB_1_5BIT);//1.5位停止位
     }
     else if(cnfp->StopBit == TwoStopBit)
     {
         usart_stop_bit_set(usart_periph,USART_STB_2BIT);//2位停止位
     }
     else
     {
         return -1;
     }
     //
     usart_hardware_flow_rts_config(usart_periph, USART_RTS_DISABLE);
     usart_hardware_flow_cts_config(usart_periph, USART_CTS_DISABLE);
     //
     usart_receive_config(usart_periph, USART_RECEIVE_ENABLE);//配置接收模块
     usart_transmit_config(usart_periph, USART_TRANSMIT_ENABLE);//配置发送模块
     usart_enable(usart_periph);//使能

     //---------------------------------- 配置接收中断 ----------------------------------
     if(cnfp->RecData_IntEnable)
     {//接收中断使能          
          switch(usart_periph)
          {
               case USART0:
                    ReceFunP0 = cnfp->Recfp;//配置接收回调函数
                    break;
               case USART1:
                    ReceFunP1 = cnfp->Recfp;//配置接收回调函数
                    break;
               case USART2:
                    ReceFunP2 = cnfp->Recfp;//配置接收回调函数
                    break;
               default:
                    return -1;
          }
          usart_flag_clear(usart_periph, USART_FLAG_RBNE);
          usart_interrupt_enable(usart_periph, USART_INT_RBNE);
     }
     else
     {//接收中断禁止
          usart_interrupt_disable(usart_periph, USART_INT_RBNE);
          switch(usart_periph)
          {
               case USART0:
                    ReceFunP0 = (USARTReceEventCBF)NULLFP;
                    break;
               case USART1:
                    ReceFunP1 = (USARTReceEventCBF)NULLFP;
                    break;
               case USART2:
                    ReceFunP2 = (USARTReceEventCBF)NULLFP;
                    break;
               default:
                    return -1;
          }
     }

     //---------------------------------- 配置接收错误中断 ----------------------------------
     usart_flag_clear(usart_periph, USART_FLAG_ORERR);//清除溢出错误
     usart_flag_clear(usart_periph, USART_FLAG_NERR);//清除噪声错误
     usart_flag_clear(usart_periph, USART_FLAG_FERR);//清除帧错误
     usart_flag_clear(usart_periph, USART_FLAG_PERR);//清除校验错误 

     //---------------------------------- 配置发送中断 ----------------------------------
     if(cnfp->SendOK_IntEnable)
     {//发送完成中断使能
          switch(usart_periph)
          {
               case USART0:
                    SendOKFunP0 = cnfp->SendOKfp;//配置发送完成回调函数
                    break;
               case USART1:
                    SendOKFunP1 = cnfp->SendOKfp;//配置发送完成回调函数
                    break;
               case USART2:
                    SendOKFunP2 = cnfp->SendOKfp;//配置发送完成回调函数
                    break;
               default:
                    return -1;
          }
          usart_flag_clear(usart_periph, USART_FLAG_TC);
          if(cnfp->SendINTDefaultState)
          {
               usart_interrupt_enable(usart_periph, USART_INT_TC);
          }
     }
     else
     {//发送完成中断禁止
          usart_interrupt_disable(usart_periph, USART_INT_TC);
          switch(usart_periph)
          {
               case USART0:
                    SendOKFunP0 = NULLFP;
                    break;
               case USART1:
                    SendOKFunP1 = NULLFP;
                    break;
               case USART2:
                    SendOKFunP2 = NULLFP;
                    break;
               default:
                    return -1;
          }
     }

     return 0;
}

// //接收硬流控输出XON
// static void USARTn_HW_Flowcontrol_ON(RHWFlowCrl *rhwfcp)
// {
//      if(rhwfcp->FEnable)
//      {
//           if(rhwfcp->RHWFlowXonLevel)
//           {
//                PORT_SetBits((en_port_t)(rhwfcp->RFlowcontrolp),(en_pin_t)((u16)1 << (rhwfcp->RFlowcontroln)));
//           }
//           else
//           {
//                PORT_ResetBits((en_port_t)(rhwfcp->RFlowcontrolp),(en_pin_t)((u16)1 << (rhwfcp->RFlowcontroln)));
//           }
//      }
// }

// //接收硬流控输出XOFF
// static void USARTn_HW_Flowcontrol_OFF(RHWFlowCrl *rhwfcp)
// {
//      if(rhwfcp->FEnable)
//      {
//           if(rhwfcp->RHWFlowXonLevel)
//           {
//                PORT_ResetBits((en_port_t)(rhwfcp->RFlowcontrolp),(en_pin_t)((u16)1 << (rhwfcp->RFlowcontroln)));
//           }
//           else
//           {
//                PORT_SetBits((en_port_t)(rhwfcp->RFlowcontrolp),(en_pin_t)((u16)1 << (rhwfcp->RFlowcontroln)));
//           }
//      }
// }









