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
     【1】、使用的引脚不是固定的，具体要看手册和原理图上安排的引脚
            所以，配置时，开放配置为不同的引脚 。
     【2】、中断号(如RecDataIntNum、RecErrIntNum)要注意，不能和其他模块使用的中断号相同
     【3】、优先级值越小，越高。


使用例子：
     UsartCnfType usartconfig;

     usartconfig.upn = Usart3;
     usartconfig.bps = 115200;
     usartconfig.rxp = GPORTE;
     usartconfig.rxn = GP04;
     usartconfig.txp = GPORTE;
     usartconfig.txn = GP05;
     usartconfig.Recfp = usar3;
     usartconfig.RecDataIntNum = Int000_IRQn;
     usartconfig.RecData_Pri = 15; 
     usartconfig.RecErrIntNum = Int001_IRQn;       
     usartconfig.RecErr_Pri = 0;        
     DRI_USARTn_Config(&usartconfig);
     DRI_USART3_SendSTR("Hello World! 你好世界!\r\n");

     void usar3(u8 rd)
     {
          DRI_USART3_SendByte(rd);
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

typedef void(*NULLFP1)(u8);//定义函数指针类型

#define XON    0x11
#define XOFF   0x13

static void (*ReceFunP0)(u8) = (NULLFP1)NULLFP;
// static void (*ReceFunP1)(u8) = (NULLFP1)NULLFP;//函数指针类型强制转换
// static void (*ReceFunP2)(u8) = (NULLFP1)NULLFP;
// static void (*ReceFunP3)(u8) = (NULLFP1)NULLFP;
// static void (*ReceFunP4)(u8) = (NULLFP1)NULLFP;
// static void (*ReceFunP5)(u8) = (NULLFP1)NULLFP;
// static void (*ReceFunP6)(u8) = (NULLFP1)NULLFP;
// static void (*ReceFunP7)(u8) = (NULLFP1)NULLFP;

static void (*SendOKFunP0)(void) = NULLFP;
// static void (*SendOKFunP1)(void) = NULLFP;
// static void (*SendOKFunP2)(void) = NULLFP;
// static void (*SendOKFunP3)(void) = NULLFP;
// static void (*SendOKFunP4)(void) = NULLFP;
// static void (*SendOKFunP5)(void) = NULLFP;
// static void (*SendOKFunP6)(void) = NULLFP;
// static void (*SendOKFunP7)(void) = NULLFP;

static RHWFlowCrl RHWFC0;//,RHWFC1,RHWFC2,RHWFC3,RHWFC4,RHWFC5,RHWFC6,RHWFC7;//接收硬流件
/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的内部函数申明区------------------------------------                          *
*                                                                                                                                           *
********************************************************************************************************************************************/
static s8 USARTn_Config(u32 usart_periph,DRI_USARTCnfType *cnfp);

// static void USART1_ErrIrqCallback(void);
// static void USART1_RxCallback(void);
// static void USART1_TCxCallback(void);
// static void USART2_ErrIrqCallback(void);
// static void USART2_RxCallback(void);
// static void USART2_TCxCallback(void);
// static void USART3_ErrIrqCallback(void);
// static void USART3_RxCallback(void);
// static void USART3_TCxCallback(void);
// static void USART4_ErrIrqCallback(void);
// static void USART4_RxCallback(void);
// static void USART4_TCxCallback(void);
// static void USARTn_HW_Flowcontrol_ON(RHWFlowCrl *rhwfcp);
// static void USARTn_HW_Flowcontrol_OFF(RHWFlowCrl *rhwfcp);
// static s8 USARTn_12PinCheck(DRI_USARTCnfType *cnfp);
// static s8 USARTn_34PinCheck(DRI_USARTCnfType *cnfp);
//static s8 USARTn_Config(DRI_USARTCnfType *cnfp,USARTnPara *usartp);

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的系统函数代码区------------------------------------                          *
*                                                                                                                                           *
********************************************************************************************************************************************/
/***************************************************************************
* 函 数 名: DRI_USART0_Config
* 功能描述：USART0配置函数
* 入口参数：
            DRI_USARTCnfType *cnfp：配置结构体指针
* 出口参数：
            -1:表示配置失败
            0:表示配置成功
* 注意事项: 

* 例     如:
* 修改记录 :
*           2025-08-29 BY:YJX
***************************************************************************/
s8 DRI_USART0_Config(DRI_USARTCnfType *cnfp)
{
     s8 s8result;
     //
     NVIC_DisableIRQ(USART0_IRQn);//禁止中断，防止配置过程中产生中断
     //

     switch(cnfp->PinSelect)
     {
          case USART0_RX_PA10_TX_PA9:
               //使能GPIOA时钟
               rcu_periph_clock_enable(RCU_GPIOA);              
               //PA9 Tx引脚配置
               //PA9引脚复用功能配置
               gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);
               //PA10 Rx引脚配置
               //PA10引脚复用功能配置
               gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_10);
               break;
          default:
               return -1;
     }    

     //使能USART0时钟
     rcu_periph_clock_enable(RCU_USART0);
     //
     RHWFC0.FEnable = cnfp->rhwfc.FEnable;
     RHWFC0.RFlowcontroln = cnfp->rhwfc.RFlowcontroln;
     RHWFC0.RFlowcontrolp = cnfp->rhwfc.RFlowcontrolp;
     RHWFC0.RHWFlowDefaultLevel = cnfp->rhwfc.RHWFlowDefaultLevel;
     RHWFC0.RHWFlowXonLevel = cnfp->rhwfc.RHWFlowXonLevel;
     //接收硬流控引脚 配置
     if(RHWFC0.FEnable)
     {//使能硬流控
          //接收硬流控引脚配置为输出
          DRI_GPIO_Config(RHWFC0.RFlowcontrolp,RHWFC0.RFlowcontroln,GDOut);
          //配置默认流控电平
          if(RHWFC0.RHWFlowDefaultLevel)
          {
               DRI_GPIO_OUT_1(RHWFC0.RFlowcontrolp,RHWFC0.RFlowcontroln);
          }
          else
          {
               DRI_GPIO_OUT_0(RHWFC0.RFlowcontrolp,RHWFC0.RFlowcontroln);
          }          
     }

     //串口模块配置
     s8result = USARTn_Config(USART0,cnfp);
     //
     if((cnfp->IntPri < 16) && (s8result == 0))
     {//允许中断
          NVIC_SetPriority(USART0_IRQn, cnfp->IntPri);//设置中断优先级
          NVIC_ClearPendingIRQ(USART0_IRQn);//清除中断标志
          NVIC_EnableIRQ(USART0_IRQn);//使能中断
          //nvic_irq_enable(USART0_IRQn, 2U, 0U);
     }
          
     return s8result;
}


void USART0_IRQHandler(void)
{
     u8 u8temp;
     
     if(RESET != usart_flag_get(USART0, USART_FLAG_RBNE))
     {
          if(RESET != usart_interrupt_flag_get(USART0, USART_INT_FLAG_RBNE))
          {
               u8temp = usart_data_receive(USART0);//读取接收数据后会自动清除标志位(usart_data_receive(USART0) & 0x7F);
               ReceFunP0(u8temp);//
          }
          else
          {
               u8temp = usart_data_receive(USART0);//读取接收数据后会自动清除标志位(usart_data_receive(USART0) & 0x7F);
          }
     }
     
     if(RESET != usart_flag_get(USART0, USART_FLAG_TC))
     {
          usart_flag_clear(USART0, USART_FLAG_TC);
          //发送中断
          if(RESET != usart_interrupt_flag_get(USART0, USART_INT_FLAG_TC))
          {
               SendOKFunP0();//处理发送中断
          }
     }

     //溢出错误
     if(RESET != usart_flag_get(USART0, USART_FLAG_ORERR))
     {
          usart_flag_clear(USART0, USART_FLAG_ORERR);
          u8temp = usart_data_receive(USART0);//读掉错误数据          
     }

     //噪声错误
     if(RESET != usart_flag_get(USART0, USART_FLAG_NERR))
     {
          usart_flag_clear(USART0, USART_FLAG_NERR);
          u8temp = usart_data_receive(USART0);//读掉错误数据          
     }

     //帧错误
     if(RESET != usart_flag_get(USART0, USART_FLAG_FERR))
     {
          usart_flag_clear(USART0, USART_FLAG_FERR);
          u8temp = usart_data_receive(USART0);//读掉错误数据          
     }

     //奇偶错误
     if(RESET != usart_flag_get(USART0, USART_FLAG_PERR))
     {
          usart_flag_clear(USART0, USART_FLAG_PERR);
          u8temp = usart_data_receive(USART0);//读掉错误数据          
     }
}//












// /***************************************************************************
// * 函 数 名: DRI_USART2_Config
// * 功能描述：USART2配置函数
// * 入口参数：
//             DRI_USARTCnfType *cnfp：配置结构体指针
// * 出口参数：
//             -1:表示配置失败
//             0:表示配置成功
// * 注意事项: 

// * 例     如:
// * 修改记录 :
// *           2022-06-06 BY:YJX
// ***************************************************************************/
// s8 DRI_USART2_Config(DRI_USARTCnfType *cnfp)
// {
//      USARTnPara usartpara;

//      if(USARTn_12PinCheck(cnfp))
//      {//引脚位置错误
//           return -1;
//      }  
     
//      usartpara.UsartCh = M4_USART2;               
//      usartpara.UsartRxFunc = Func_Usart2_Rx;
//      usartpara.UsartTxFunc = Func_Usart2_Tx;
//      usartpara.u32Fcg1Periph = PWC_FCG1_PERIPH_USART2;
//      usartpara.UsartRiSrc = INT_USART2_RI;
//      usartpara.UsartEiSrc = INT_USART2_EI;
//      usartpara.UsartTCiSrc = INT_USART2_TCI;
//      usartpara.TCIcbf = USART2_TCxCallback;
//      usartpara.RIcbf = USART2_RxCallback;
//      usartpara.EIcbf = USART2_ErrIrqCallback;
//      ReceFunP2 = cnfp->Recfp;
//      SendOKFunP2 = cnfp->SendOKfp;
//      RHWFC2 = cnfp->rhwfc;
//      return USARTn_Config(cnfp,&usartpara);
// }

// /***************************************************************************
// * 函 数 名: DRI_USART3_Config
// * 功能描述：USART3配置函数
// * 入口参数：
//             DRI_USARTCnfType *cnfp：配置结构体指针
// * 出口参数：
//             -1:表示配置失败
//             0:表示配置成功
// * 注意事项: 

// * 例     如:
// * 修改记录 :
// *           2022-06-06 BY:YJX
// ***************************************************************************/
// s8 DRI_USART3_Config(DRI_USARTCnfType *cnfp)
// {
//      USARTnPara usartpara;

//      if(USARTn_34PinCheck(cnfp))
//      {//引脚位置错误
//           return -1;
//      }  
     
//      usartpara.UsartCh = M4_USART3;               
//      usartpara.UsartRxFunc = Func_Usart3_Rx;
//      usartpara.UsartTxFunc = Func_Usart3_Tx;
//      usartpara.u32Fcg1Periph = PWC_FCG1_PERIPH_USART3;
//      usartpara.UsartRiSrc = INT_USART3_RI;
//      usartpara.UsartEiSrc = INT_USART3_EI;
//      usartpara.UsartTCiSrc = INT_USART3_TCI;
//      usartpara.TCIcbf = USART3_TCxCallback;
//      usartpara.RIcbf = USART3_RxCallback;
//      usartpara.EIcbf = USART3_ErrIrqCallback;
//      ReceFunP3 = cnfp->Recfp;
//      SendOKFunP3 = cnfp->SendOKfp;
//      RHWFC3 = cnfp->rhwfc;
//      return USARTn_Config(cnfp,&usartpara);
// }

// /***************************************************************************
// * 函 数 名: DRI_USART4_Config
// * 功能描述：USART4配置函数
// * 入口参数：
//             DRI_USARTCnfType *cnfp：配置结构体指针
// * 出口参数：
//             -1:表示配置失败
//             0:表示配置成功
// * 注意事项: 

// * 例     如:
// * 修改记录 :
// *           2022-06-06 BY:YJX
// ***************************************************************************/
// s8 DRI_USART4_Config(DRI_USARTCnfType *cnfp)
// {
//      USARTnPara usartpara;

//      if(USARTn_34PinCheck(cnfp))
//      {//引脚位置错误
//           return -1;
//      }  
     
//      usartpara.UsartCh = M4_USART4;               
//      usartpara.UsartRxFunc = Func_Usart4_Rx;
//      usartpara.UsartTxFunc = Func_Usart4_Tx;
//      usartpara.u32Fcg1Periph = PWC_FCG1_PERIPH_USART4;
//      usartpara.UsartRiSrc = INT_USART4_RI;
//      usartpara.UsartEiSrc = INT_USART4_EI;
//      usartpara.UsartTCiSrc = INT_USART4_TCI;
//      usartpara.TCIcbf = USART4_TCxCallback;
//      usartpara.RIcbf = USART4_RxCallback;
//      usartpara.EIcbf = USART4_ErrIrqCallback;
//      ReceFunP4 = cnfp->Recfp;
//      SendOKFunP4 = cnfp->SendOKfp;
//      RHWFC4 = cnfp->rhwfc;
//      return USARTn_Config(cnfp,&usartpara);
// }


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
*           2025-09-01 BY:YJX
***************************************************************************/
void DRI_USART0_SendByte(u8 sd)
{
     while (RESET == usart_flag_get(USART0, USART_FLAG_TBE));
     usart_data_transmit(USART0, sd);
}

// /***************************************************************************
// * 函 数 名: DRI_USART1_SendByte
// * 功能描述：USART1 阻塞型 发送1个字节函数
// * 入口参数：
//             u8 sd：待发送字节
// * 出口参数：
//             无
// * 注意事项:  
//             无
// * 例     如:
// * 修改记录 :
// *           2022-06-06 BY:YJX
// ***************************************************************************/
// void DRI_USART1_SendByte(u8 sd)
// {
//      M4_USART1->DR_f.TDR = sd;//写数据到发送缓存
//      while((M4_USART1->SR & UsartTxEmpty) == 0);//等待发送完成(TXE由硬件置1清0)
// }

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
// * 函 数 名: DRI_USART2_SendByte
// * 功能描述：USART2 阻塞型 发送1个字节函数
// * 入口参数：
//             u8 sd：待发送字节
// * 出口参数：
//             无
// * 注意事项:  
//             无
// * 例     如:
// * 修改记录 :
// *           2022-06-06 BY:YJX
// ***************************************************************************/
// void DRI_USART2_SendByte(u8 sd)
// {
//      M4_USART2->DR_f.TDR = sd;//写数据到发送缓存
//      while((M4_USART2->SR & UsartTxEmpty) == 0);//等待发送完成(TXE由硬件置1清0)
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
// * 函 数 名: DRI_USART3_SendByte
// * 功能描述：USART3 阻塞型 发送1个字节函数
// * 入口参数：
//             u8 sd：待发送字节
// * 出口参数：
//             无
// * 注意事项:  
//             无
// * 例     如:
// * 修改记录 :
// *           2022-06-06 BY:YJX
// ***************************************************************************/
// void DRI_USART3_SendByte(u8 sd)
// {
//      M4_USART3->DR_f.TDR = sd;//写数据到发送缓存
//      while((M4_USART3->SR & UsartTxEmpty) == 0);//等待发送完成(TXE由硬件置1清0)
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
// * 函 数 名: DRI_USART4_SendByte
// * 功能描述：USART4 阻塞型 发送1个字节函数
// * 入口参数：
//             u8 sd：待发送字节
// * 出口参数：
//             无
// * 注意事项:  
//             无
// * 例     如:
// * 修改记录 :
// *           2022-06-06 BY:YJX
// ***************************************************************************/
// void DRI_USART4_SendByte(u8 sd)
// {
//      M4_USART4->DR_f.TDR = sd;//写数据到发送缓存
//      while((M4_USART4->SR & UsartTxEmpty) == 0);//等待发送完成(TXE由硬件置1清0)
// }

// /***************************************************************************
// * 函 数 名: DRI_USART4_NonBlockSendByte
// * 功能描述：USART4 非阻塞型 发送1个字节函数
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
// void DRI_USART4_NonBlockSendByte(u8 sd)
// {
//      M4_USART4->DR_f.TDR = sd;//写数据到发送缓存
// }

// /***************************************************************************
// * 函 数 名: DRI_USART1_SendSTR
// * 功能描述：USART1 阻塞型 发送字符串函数
// * 入口参数：
//             u8 *strp：字符串首地址
// * 出口参数：
//             无
// * 注意事项:  
//             字符串要包含\0
// * 例     如:
// * 修改记录 :
// *           2022-06-06 BY:YJX
// ***************************************************************************/
// void DRI_USART1_SendSTR(u8 *strp)
// {
//      u32 u32count = 0;
//      while(strp[u32count] != '\0')
//      {
//           M4_USART1->DR_f.TDR = strp[u32count++];//写数据到发送缓存
//           while((M4_USART1->SR & UsartTxEmpty) == 0);//等待发送完成(TXE由硬件置1清0)
//      }
// }

// /***************************************************************************
// * 函 数 名: DRI_USART2_SendSTR
// * 功能描述：USART2 阻塞型 发送字符串函数
// * 入口参数：
//             u8 *strp：字符串首地址
// * 出口参数：
//             无
// * 注意事项:  
//             字符串要包含\0
// * 例     如:
// * 修改记录 :
// *           2022-06-06 BY:YJX
// ***************************************************************************/
// void DRI_USART2_SendSTR(u8 *strp)
// {
//      u32 u32count = 0;
//      while(strp[u32count] != '\0')
//      {
//           M4_USART2->DR_f.TDR = strp[u32count++];//写数据到发送缓存
//           while((M4_USART2->SR & UsartTxEmpty) == 0);//等待发送完成(TXE由硬件置1清0)
//      }
// }

// /***************************************************************************
// * 函 数 名: DRI_USART3_SendSTR
// * 功能描述：USART3 阻塞型 发送字符串函数
// * 入口参数：
//             u8 *strp：字符串首地址
// * 出口参数：
//             无
// * 注意事项:  
//             字符串要包含\0
// * 例     如:
// * 修改记录 :
// *           2022-06-06 BY:YJX
// ***************************************************************************/
// void DRI_USART3_SendSTR(u8 *strp)
// {
//      u32 u32count = 0;
//      while(strp[u32count] != '\0')
//      {
//           M4_USART3->DR_f.TDR = strp[u32count++];//写数据到发送缓存
//           while((M4_USART3->SR & UsartTxEmpty) == 0);//等待发送完成(TXE由硬件置1清0)
//      }
// }

// /***************************************************************************
// * 函 数 名: DRI_USART4_SendSTR
// * 功能描述：USART4 阻塞型 发送字符串函数
// * 入口参数：
//             u8 *strp：字符串首地址
// * 出口参数：
//             无
// * 注意事项:  
//             字符串要包含\0
// * 例     如:
// * 修改记录 :
// *           2022-06-06 BY:YJX
// ***************************************************************************/
// void DRI_USART4_SendSTR(u8 *strp)
// {
//      u32 u32count = 0;
//      while(strp[u32count] != '\0')
//      {
//           M4_USART4->DR_f.TDR = strp[u32count++];//写数据到发送缓存
//           while((M4_USART4->SR & UsartTxEmpty) == 0);//等待发送完成(TXE由硬件置1清0)
//      }
// }


// /***************************************************************************
// * 函 数 名: DRI_USART1_SendData
// * 功能描述：USART1 阻塞型 发送数据函数
// * 入口参数：
//             u8 *sd：数据数据数组首地址
//             u16 sl：待发送字节数
// * 出口参数：
//             无
// * 注意事项:  
            
// * 例     如:
// * 修改记录 :
// *           2022-06-06 BY:YJX
// ***************************************************************************/
// void DRI_USART1_SendData(u8 *sd,u16 sl)
// {
//      u16 u16count = 0; 
//      while(u16count < sl)
//      {
//           M4_USART1->DR_f.TDR = sd[u16count++];//写数据到发送缓存
//           while((M4_USART1->SR & UsartTxEmpty) == 0);//等待发送完成(TXE由硬件置1清0)
//      }
     
// }

// /***************************************************************************
// * 函 数 名: DRI_USART2_SendData
// * 功能描述：USART2 阻塞型 发送数据函数
// * 入口参数：
//             u8 *sd：数据数据数组首地址
//             u16 sl：待发送字节数
// * 出口参数：
//             无
// * 注意事项:  
            
// * 例     如:
// * 修改记录 :
// *           2022-06-06 BY:YJX
// ***************************************************************************/
// void DRI_USART2_SendData(u8 *sd,u16 sl)
// {
//      u16 u16count = 0; 
//      while(u16count < sl)
//      {
//           M4_USART2->DR_f.TDR = sd[u16count++];//写数据到发送缓存
//           while((M4_USART2->SR & UsartTxEmpty) == 0);//等待发送完成(TXE由硬件置1清0)
//      }
     
// }

// /***************************************************************************
// * 函 数 名: DRI_USART3_SendData
// * 功能描述：USART3 阻塞型 发送数据函数
// * 入口参数：
//             u8 *sd：数据数据数组首地址
//             u16 sl：待发送字节数
// * 出口参数：
//             无
// * 注意事项:  
            
// * 例     如:
// * 修改记录 :
// *           2022-06-06 BY:YJX
// ***************************************************************************/
// void DRI_USART3_SendData(u8 *sd,u16 sl)
// {
//      u16 u16count = 0; 
//      while(u16count < sl)
//      {
//           M4_USART3->DR_f.TDR = sd[u16count++];//写数据到发送缓存
//           while((M4_USART3->SR & UsartTxEmpty) == 0);//等待发送完成(TXE由硬件置1清0)
//      }
     
// }

// /***************************************************************************
// * 函 数 名: DRI_USART4_SendData
// * 功能描述：USART4 阻塞型 发送数据函数
// * 入口参数：
//             u8 *sd：数据数据数组首地址
//             u16 sl：待发送字节数
// * 出口参数：
//             无
// * 注意事项:  
            
// * 例     如:
// * 修改记录 :
// *           2022-06-06 BY:YJX
// ***************************************************************************/
// void DRI_USART4_SendData(u8 *sd,u16 sl)
// {
//      u16 u16count = 0; 
//      while(u16count < sl)
//      {
//           M4_USART4->DR_f.TDR = sd[u16count++];//写数据到发送缓存
//           while((M4_USART4->SR & UsartTxEmpty) == 0);//等待发送完成(TXE由硬件置1清0)
//      }
     
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
// * 函 数 名: DRI_USART3_EnableReceINT
// * 功能描述：USART3 使能接收中断 函数
// * 入口参数：
//             无
// * 出口参数：
//             无
// * 注意事项:  
            
// * 例     如:
// * 修改记录 :
// *           2022-06-23 BY:YJX
// ***************************************************************************/
// void DRI_USART3_EnableReceINT(void)
// {
//      M4_USART_TypeDef *Usartx = M4_USART3;
//      u16 u16temp = Usartx->DR_f.RDR;//清除原缓存数据
//      u16temp = u16temp;
//      USART_ClearStatus(Usartx, UsartFrameErr);//清除帧错误
//      USART_ClearStatus(Usartx, UsartParityErr);//清除校验错误
//      USART_ClearStatus(Usartx, UsartOverrunErr);//清除 
//      USART_FuncCmd(Usartx, UsartRxInt, Enable);//使能接收数据和接收错误中断
// }

// /***************************************************************************
// * 函 数 名: DRI_USART4_EnableReceINT
// * 功能描述：USART4 使能接收中断 函数
// * 入口参数：
//             无
// * 出口参数：
//             无
// * 注意事项:  
            
// * 例     如:
// * 修改记录 :
// *           2022-06-23 BY:YJX
// ***************************************************************************/
// void DRI_USART4_EnableReceINT(void)
// {
//      M4_USART_TypeDef *Usartx = M4_USART4;
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
// * 函 数 名: DRI_USART3_DisableReceINT
// * 功能描述：USART3 禁止接收中断 函数
// * 入口参数：
//             无
// * 出口参数：
//             无
// * 注意事项:  
            
// * 例     如:
// * 修改记录 :
// *           2022-06-23 BY:YJX
// ***************************************************************************/
// void DRI_USART3_DisableReceINT(void)
// {
//      USART_FuncCmd(M4_USART3, UsartRxInt, Disable);//禁止接收数据和接收错误中断
// }

// /***************************************************************************
// * 函 数 名: DRI_USART4_DisableReceINT
// * 功能描述：USART4 禁止接收中断 函数
// * 入口参数：
//             无
// * 出口参数：
//             无
// * 注意事项:  
            
// * 例     如:
// * 修改记录 :
// *           2022-06-23 BY:YJX
// ***************************************************************************/
// void DRI_USART4_DisableReceINT(void)
// {
//      USART_FuncCmd(M4_USART4, UsartRxInt, Disable);//禁止接收数据和接收错误中断
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
// * 函 数 名: DRI_USART3_EnableSendOKINT
// * 功能描述：USART3 使能发送完成中断 函数
// * 入口参数：
//             无
// * 出口参数：
//             无
// * 注意事项:  
            
// * 例     如:
// * 修改记录 :
// *           2022-06-23 BY:YJX
// ***************************************************************************/
// void DRI_USART3_EnableSendOKINT(void)
// {
//      USART_FuncCmd(M4_USART3, UsartTxCmpltInt, Enable);//使能发送完成中断
// }

// /***************************************************************************
// * 函 数 名: DRI_USART4_EnableSendOKINT
// * 功能描述：USART4 使能发送完成中断 函数
// * 入口参数：
//             无
// * 出口参数：
//             无
// * 注意事项:  
            
// * 例     如:
// * 修改记录 :
// *           2022-06-23 BY:YJX
// ***************************************************************************/
// void DRI_USART4_EnableSendOKINT(void)
// {
//      USART_FuncCmd(M4_USART4, UsartTxCmpltInt, Enable);//使能发送完成中断
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
// * 函 数 名: DRI_USART3_DisableSendOKINT
// * 功能描述：USART3 禁止发送完成中断 函数
// * 入口参数：
//             无
// * 出口参数：
//             无
// * 注意事项:  
            
// * 例     如:
// * 修改记录 :
// *           2022-06-23 BY:YJX
// ***************************************************************************/
// void DRI_USART3_DisableSendOKINT(void)
// {
//      USART_FuncCmd(M4_USART3, UsartTxCmpltInt, Disable);//禁止发送完成中断
// }

// /***************************************************************************
// * 函 数 名: DRI_USART4_DisableSendOKINT
// * 功能描述：USART4 禁止发送完成中断 函数
// * 入口参数：
//             无
// * 出口参数：
//             无
// * 注意事项:  
            
// * 例     如:
// * 修改记录 :
// *           2022-06-23 BY:YJX
// ***************************************************************************/
// void DRI_USART4_DisableSendOKINT(void)
// {
//      USART_FuncCmd(M4_USART4, UsartTxCmpltInt, Disable);//禁止发送完成中断
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
// * 函 数 名: DRI_USART3_XON
// * 功能描述：USART3 阻塞型 发送软流控XON数据 函数
// * 入口参数：
//             无
// * 出口参数：
//             无
// * 注意事项:  
            
// * 例     如:
// * 修改记录 :
// *           2022-06-23 BY:YJX
// ***************************************************************************/
// void DRI_USART3_XON(void)
// {
//      DRI_USART3_SendByte(XON);
// }

// /***************************************************************************
// * 函 数 名: DRI_USART4_XON
// * 功能描述：USART4 阻塞型 发送软流控XON数据 函数
// * 入口参数：
//             无
// * 出口参数：
//             无
// * 注意事项:  
            
// * 例     如:
// * 修改记录 :
// *           2022-06-23 BY:YJX
// ***************************************************************************/
// void DRI_USART4_XON(void)
// {
//      DRI_USART4_SendByte(XON);
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
// * 函 数 名: DRI_USART3_XOFF
// * 功能描述：USART3 阻塞型 发送软流控XOFF数据 函数
// * 入口参数：
//             无
// * 出口参数：
//             无
// * 注意事项:  
            
// * 例     如:
// * 修改记录 :
// *           2022-06-23 BY:YJX
// ***************************************************************************/
// void DRI_USART3_XOFF(void)
// {
//      DRI_USART3_SendByte(XOFF);
// }

// /***************************************************************************
// * 函 数 名: DRI_USART4_XOFF
// * 功能描述：USART4 阻塞型 发送软流控XOFF数据 函数
// * 入口参数：
//             无
// * 出口参数：
//             无
// * 注意事项:  
            
// * 例     如:
// * 修改记录 :
// *           2022-06-23 BY:YJX
// ***************************************************************************/
// void DRI_USART4_XOFF(void)
// {
//      DRI_USART4_SendByte(XOFF);
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
// * 函 数 名: DRI_USART3_HW_Flowcontrol_ON
// * 功能描述：USART3 控制接收硬流控输出 XON电平 函数
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
// void DRI_USART3_HW_Flowcontrol_ON(void)
// {
//      USARTn_HW_Flowcontrol_ON(&RHWFC3);
// }

// /***************************************************************************
// * 函 数 名: DRI_USART4_HW_Flowcontrol_ON
// * 功能描述：USART4 控制接收硬流控输出 XON电平 函数
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
// void DRI_USART4_HW_Flowcontrol_ON(void)
// {
//      USARTn_HW_Flowcontrol_ON(&RHWFC4);
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

// /***************************************************************************
// * 函 数 名: DRI_USART3_HW_Flowcontrol_OFF
// * 功能描述：USART3 控制接收硬流控输出 XOFF电平 函数
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
// void DRI_USART3_HW_Flowcontrol_OFF(void)
// {
//      USARTn_HW_Flowcontrol_OFF(&RHWFC3);
// }

// /***************************************************************************
// * 函 数 名: DRI_USART4_HW_Flowcontrol_OFF
// * 功能描述：USART4 控制接收硬流控输出 XOFF电平 函数
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
// void DRI_USART4_HW_Flowcontrol_OFF(void)
// {
//      USARTn_HW_Flowcontrol_OFF(&RHWFC4);
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
                    ReceFunP0 = (NULLFP1)NULLFP;
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
               default:
                    return -1;
          }
     }

     return 0;
}

// //Usart1错误回调处理函数
// static void USART1_ErrIrqCallback(void)
// {
//      M4_USART_TypeDef *USARTx = M4_USART1;
//      if(USART_GetStatus(USARTx, UsartFrameErr))
//      {
//           USART_ClearStatus(USARTx, UsartFrameErr);
//      }

//      if(USART_GetStatus(USARTx, UsartParityErr))
//      {
//           USART_ClearStatus(USARTx, UsartParityErr);
//      }

//      if(USART_GetStatus(USARTx, UsartOverrunErr))
//      {
//           USART_ClearStatus(USARTx, UsartOverrunErr);
//      }
// }

// //Usart1数据接收回调处理函数
// static void USART1_RxCallback(void)
// {
//      u16 u16rd = M4_USART1->DR_f.RDR;//USART_RecData(M4_USART1);//读取收到的数据
//      //RXNE由硬件置1，读取数据后，由硬件清0
//      ReceFunP1((u8)u16rd);     
// }

// //Usart1发送完成回调处理函数
// static void USART1_TCxCallback(void)
// {
//      //此处标志寄存器由硬件根据移位寄存器情况(即重新开始发送时，硬件会自动清0)
//      SendOKFunP1();
// }

// //Usart2错误回调处理函数
// static void USART2_ErrIrqCallback(void)
// {
//      M4_USART_TypeDef *USARTx = M4_USART2;
//      if(USART_GetStatus(USARTx, UsartFrameErr))
//      {
//           USART_ClearStatus(USARTx, UsartFrameErr);
//      }

//      if(USART_GetStatus(USARTx, UsartParityErr))
//      {
//           USART_ClearStatus(USARTx, UsartParityErr);
//      }

//      if(USART_GetStatus(USARTx, UsartOverrunErr))
//      {
//           USART_ClearStatus(USARTx, UsartOverrunErr);
//      }
// }

// //Usart2数据接收回调处理函数
// static void USART2_RxCallback(void)
// {
//      u16 u16rd = M4_USART2->DR_f.RDR;//USART_RecData(M4_USART2);//读取收到的数据
//      //RXNE由硬件置1，读取数据后，由硬件清0
//      ReceFunP2((u8)u16rd);     
// }

// //Usart2发送完成回调处理函数
// static void USART2_TCxCallback(void)
// {
//      //此处标志寄存器由硬件根据移位寄存器情况(即重新开始发送时，硬件会自动清0)
//      SendOKFunP2();
// }

// //Usart3错误回调处理函数
// static void USART3_ErrIrqCallback(void)
// {
//      M4_USART_TypeDef *USARTx = M4_USART3;
//      if(USART_GetStatus(USARTx, UsartFrameErr))
//      {
//           USART_ClearStatus(USARTx, UsartFrameErr);
//      }

//      if(USART_GetStatus(USARTx, UsartParityErr))
//      {
//           USART_ClearStatus(USARTx, UsartParityErr);
//      }

//      if(USART_GetStatus(USARTx, UsartOverrunErr))
//      {
//           USART_ClearStatus(USARTx, UsartOverrunErr);
//      }
// }

// //Usart3数据接收回调处理函数
// static void USART3_RxCallback(void)
// {
//      u16 u16rd = M4_USART3->DR_f.RDR;//USART_RecData(M4_USART3);//读取收到的数据
//      //RXNE由硬件置1，读取数据后，由硬件清0
//      ReceFunP3((u8)u16rd);     
// }

// //Usart3发送完成回调处理函数
// static void USART3_TCxCallback(void)
// {
//      //此处标志寄存器由硬件根据移位寄存器情况(即重新开始发送时，硬件会自动清0)
//      SendOKFunP3();
// }

// //Usart4错误回调处理函数
// static void USART4_ErrIrqCallback(void)
// {
//      M4_USART_TypeDef *USARTx = M4_USART4;
//      if(USART_GetStatus(USARTx, UsartFrameErr))
//      {
//           USART_ClearStatus(USARTx, UsartFrameErr);
//      }

//      if(USART_GetStatus(USARTx, UsartParityErr))
//      {
//           USART_ClearStatus(USARTx, UsartParityErr);
//      }

//      if(USART_GetStatus(USARTx, UsartOverrunErr))
//      {
//           USART_ClearStatus(USARTx, UsartOverrunErr);
//      }
// }

// //Usart4数据接收回调处理函数
// static void USART4_RxCallback(void)
// {
//      u16 u16rd = M4_USART4->DR_f.RDR;//USART_RecData(M4_USART4);//读取收到的数据
//      //RXNE由硬件置1，读取数据后，由硬件清0
//      ReceFunP4((u8)u16rd);     
// }

// //Usart4发送完成回调处理函数
// static void USART4_TCxCallback(void)
// {
//      //此处标志寄存器由硬件根据移位寄存器情况(即重新开始发送时，硬件会自动清0)
//      SendOKFunP4();
// }

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


// //USART1和USART2引脚判断
// //返回小于0失败 等于0成功
// static s8 USARTn_12PinCheck(DRI_USARTCnfType *cnfp)
// {//不作引脚冲突判断
//      u8 USART12[6][16] = {//USART1和USART2模块Group1的引脚有以下
//                         //0,1,2,3,4,5,6,7,8,9,A,B,C,D,E,F
//                          {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},//PA
//                          {1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0},//PB
//                          {1,1,1,1,1,1,0,0,0,1,1,1,1,0,0,0},//PC
//                          {1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0},//PD
//                          {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//PE
//                          {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} //PH
//                           };
//      if(USART12[cnfp->txp][cnfp->txn] && 
//         USART12[cnfp->rxp][cnfp->rxn])
//      {
//           return 0;
//      }
//      return -1;
// }

// //USART3和USART4引脚判断
// //返回小于0失败 等于0成功
// static s8 USARTn_34PinCheck(DRI_USARTCnfType *cnfp)
// {//不作引脚冲突判断
//      u8 USART34[6][16] = {//USART3和USART4模块Group2的引脚有以下
//                         //0,1,2,3,4,5,6,7,8,9,A,B,C,D,E,F
//                          {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},//PA
//                          {0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1},//PB
//                          {0,0,0,0,0,0,1,1,1,0,0,0,0,1,0,0},//PC
//                          {0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0},//PD
//                          {1,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1},//PE
//                          {0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0} //PH
//                           };
//      if(USART34[cnfp->txp][cnfp->txn] && 
//         USART34[cnfp->rxp][cnfp->rxn])
//      {
//           return 0;
//      }
//      return -1;
// }

// //USART配置函数
// //返回小于0失败 等于0成功
// static s8 USARTn_Config(DRI_USARTCnfType *cnfp,USARTnPara *usartp)
// {
//      stc_port_init_t pstcPortInit;
//      stc_irq_regi_conf_t stcIrqRegiCfg;
//      stc_usart_uart_init_t m_stcInitCfg;
//      u8 u8temp;                  

//      //硬流控引脚初始化
//      if(cnfp->rhwfc.FEnable)
//      {
//           MEM_ZERO_STRUCT(pstcPortInit); 
//           pstcPortInit.enPinMode = Pin_Mode_Out;
//           pstcPortInit.enExInt = Disable;
//           pstcPortInit.enPullUp = Enable; 
//           if(PORT_Init((en_port_t)(cnfp->rhwfc.RFlowcontrolp),(en_pin_t)((u16)1 << (cnfp->rhwfc.RFlowcontroln)),&pstcPortInit))
//           {
//                return -2;
//           }
//           DRI_ComDriver_UserPin((cnfp->rhwfc.RFlowcontrolp),(cnfp->rhwfc.RFlowcontroln));
//           if(cnfp->rhwfc.RHWFlowDefaultLevel)
//           {//默认高电平
//                PORT_SetBits((en_port_t)(cnfp->rhwfc.RFlowcontrolp),(en_pin_t)((u16)1 << (cnfp->rhwfc.RFlowcontroln)));
//           }
//           else
//           {//默认低电平
//                PORT_ResetBits((en_port_t)(cnfp->rhwfc.RFlowcontrolp),(en_pin_t)((u16)1 << (cnfp->rhwfc.RFlowcontroln)));
//           }
//      }     
     
//      //使能外设时钟
//      PWC_Fcg1PeriphClockCmd(usartp->u32Fcg1Periph, Enable);
//      //初始化USART的IO
//      PORT_SetFunc((en_port_t)cnfp->rxp, (en_pin_t)(1 << cnfp->rxn), usartp->UsartRxFunc, Disable);
//      PORT_SetFunc((en_port_t)cnfp->txp, (en_pin_t)(1 << cnfp->txn), usartp->UsartTxFunc, Disable);
//      DRI_ComDriver_UserPin(cnfp->rxp,cnfp->rxn);
//      DRI_ComDriver_UserPin(cnfp->txp,cnfp->txn);
//      //初始化USART
//      for(u8temp = 0;u8temp < 4;u8temp++)
//      {
//           MEM_ZERO_STRUCT(m_stcInitCfg);
//           m_stcInitCfg.enDataLength = (en_usart_data_len_t)cnfp->DataBit;//数据位
//           m_stcInitCfg.enStopBit = (en_usart_stop_bit_t)cnfp->StopBit;//停止位
//           m_stcInitCfg.enParity = (en_usart_parity_t)cnfp->Parity;//校验方式
//           m_stcInitCfg.enClkMode = UsartIntClkCkNoOutput;//选择内部时钟源 且 不输出     
//           m_stcInitCfg.enDetectMode = UsartStartBitFallEdge;//起始位侦测模式为下降沿
//           m_stcInitCfg.enDirection = UsartDataLsbFirst;//低位在前
//           m_stcInitCfg.enSampleMode = UsartSampleBit8;//Usart采样方式
//           //m_stcInitCfg.enHwFlow = UsartRtsEnable;//硬流控 
//           m_stcInitCfg.enClkDiv = (en_usart_clk_div_t)u8temp;  
//           USART_DeInit(usartp->UsartCh);
//           if(USART_UART_Init(usartp->UsartCh, &m_stcInitCfg) != Ok)
//           {
//                return -3;
//           }    
//           //设置波特率
//           if(USART_SetBaudrate(usartp->UsartCh, cnfp->bps) == Ok)
//           {
//                break;
//           }
//      }
//      if(u8temp >= 4)
//      {//波特率异常
//           return -4;
//      }
    
//      //设置接收数据中断 和 接收错误中断
//      if((cnfp->RecData_Pri < 16) && (cnfp->RecErr_Pri < 16))
//      {
//      //----接收数据中断
//           //DRI_ComDriver_EnableINT_IER(cnfp->RecDataIntNum);
//           //DRI_ComDriver_IntSelReset(cnfp->RecDataIntNum);
//           DRI_ComDriver_UserInt(cnfp->RecDataIntNum);
          
//           stcIrqRegiCfg.enIRQn = cnfp->RecDataIntNum;//接收rx中断放在0中断号
//           stcIrqRegiCfg.pfnCallback = usartp->RIcbf;//接收回调函数
//           stcIrqRegiCfg.enIntSrc = usartp->UsartRiSrc;
//           enIrqRegistration(&stcIrqRegiCfg);
//           NVIC_SetPriority(stcIrqRegiCfg.enIRQn, cnfp->RecData_Pri);
//           NVIC_ClearPendingIRQ(stcIrqRegiCfg.enIRQn);
//           NVIC_EnableIRQ(stcIrqRegiCfg.enIRQn);
          
//      //----接收错误中断
//           //DRI_ComDriver_EnableINT_IER(cnfp->RecErrIntNum);
//           //DRI_ComDriver_IntSelReset(cnfp->RecErrIntNum);
//           DRI_ComDriver_UserInt(cnfp->RecErrIntNum);
          
//           stcIrqRegiCfg.enIRQn = cnfp->RecErrIntNum;
//           stcIrqRegiCfg.pfnCallback = usartp->EIcbf;//接收错误回调函数
//           stcIrqRegiCfg.enIntSrc = usartp->UsartEiSrc;
//           enIrqRegistration(&stcIrqRegiCfg);
//           USART_ClearStatus(usartp->UsartCh, UsartFrameErr);//清除帧错误
//           USART_ClearStatus(usartp->UsartCh, UsartParityErr);//清除校验错误
//           USART_ClearStatus(usartp->UsartCh, UsartOverrunErr);//清除          
//           NVIC_SetPriority(stcIrqRegiCfg.enIRQn, cnfp->RecErr_Pri);//高优先级的接收错误处理
//           NVIC_ClearPendingIRQ(stcIrqRegiCfg.enIRQn);
//           NVIC_EnableIRQ(stcIrqRegiCfg.enIRQn);
          
//      //-----使能中断(接收数据中断 和 接收错误中断 使用的是同一个中断开关使能位，详见手册P683)
//           USART_FuncCmd(usartp->UsartCh, UsartRxInt, Enable);//使能接收数据和接收错误中断
//      }


//      //设置发送中断
//      if(cnfp->SendOK_Pri < 16)
//      {
//           //DRI_ComDriver_EnableINT_IER(cnfp->SendOKIntNum);
//           //DRI_ComDriver_IntSelReset(cnfp->SendOKIntNum);
//           DRI_ComDriver_UserInt(cnfp->SendOKIntNum);
          
//           stcIrqRegiCfg.enIRQn = cnfp->SendOKIntNum;
//           stcIrqRegiCfg.pfnCallback = usartp->TCIcbf;//发送完成中断回调函数
//           stcIrqRegiCfg.enIntSrc = usartp->UsartTCiSrc;
//           enIrqRegistration(&stcIrqRegiCfg);
//           USART_ClearStatus(usartp->UsartCh, UsartTxComplete);//清除发送完成标志
//           NVIC_SetPriority(stcIrqRegiCfg.enIRQn, DDL_IRQ_PRIORITY_DEFAULT);
//           NVIC_ClearPendingIRQ(stcIrqRegiCfg.enIRQn);
//           NVIC_EnableIRQ(stcIrqRegiCfg.enIRQn);
//           if(cnfp->SendINTDefaultState)
//           {
//                USART_FuncCmd(usartp->UsartCh, UsartTxCmpltInt, Enable);//使能发送完成中断
//           }
//      }
     
//      //使能发送模块功能     
//      USART_FuncCmd(usartp->UsartCh, UsartTx, Enable);//使能发送模块
//      //使能接收模块
//      USART_FuncCmd(usartp->UsartCh, UsartRx, Enable);//使能接收模块 
    
//      return 0;
// }








