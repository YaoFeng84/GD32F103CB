/********************************************************************************************************************************************
*                                                                                                                                           *
*              ---------------------------------以下是模块的修改记录区-----------------------------------------                             *
*                                                                                                                                           *
********************************************************************************************************************************************/
/**********************************************
 * 内容：
 * 日期：2025-09-24
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

*/
#include "DRI_CAN.h"
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
typedef struct 
{
     u32 bps;
     u16 prescaler;
     u8 bs1;
     u8 bs2;
}CAN_BpsParaType;

//48MHz的CAN时钟下，波特率参数
static const CAN_BpsParaType CAN_48M_BpsPara[] =
{
     {.bps = 1000000, .prescaler = 8,   .bs1 = 4, .bs2 = 1}, //1Mbps
     {.bps = 800000,  .prescaler = 10,  .bs1 = 4, .bs2 = 1}, //800Kbps
     {.bps = 500000,  .prescaler = 16,  .bs1 = 4, .bs2 = 1}, //500Kbps
     {.bps = 400000,  .prescaler = 20,  .bs1 = 4, .bs2 = 1}, //400Kbps
     {.bps = 250000,  .prescaler = 12,  .bs1 = 12,.bs2 = 3}, //250Kbps
     {.bps = 200000,  .prescaler = 15,  .bs1 = 12,.bs2 = 3}, //200Kbps
     {.bps = 100000,  .prescaler = 30,  .bs1 = 12,.bs2 = 3}, //100Kbps
     {.bps = 50000,   .prescaler = 60,  .bs1 = 12,.bs2 = 3}, //50Kbps
     {.bps = 40000,   .prescaler = 75,  .bs1 = 12,.bs2 = 3}, //40Kbps
     {.bps = 25000,   .prescaler = 120, .bs1 = 12,.bs2 = 3}, //25Kbps
     {.bps = 20000,   .prescaler = 150, .bs1 = 12,.bs2 = 3}, //20Kbps
     {.bps = 15000,   .prescaler = 200, .bs1 = 12,.bs2 = 3}, //15Kbps
     {.bps = 10000,   .prescaler = 300, .bs1 = 12,.bs2 = 3}, //10Kbps
     {.bps = 5000,    .prescaler = 600, .bs1 = 12,.bs2 = 3} //5Kbps
};
//
static DRI_CAN_ReceCBFType CAN0_ReceCBF = NULL;
/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的内部函数申明区------------------------------------                          *
*                                                                                                                                           *
********************************************************************************************************************************************/
static s8 CAN_CalculateCANParameters(u32 baudrate,u16 *prescaler,u8 *bs1,u8 *bs2);

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的系统函数代码区------------------------------------                          *
*                                                                                                                                           *
********************************************************************************************************************************************/
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
s8 DRI_CANx_Config(DRI_CANCnfType *cnf)
{
     can_parameter_struct        can_parameter;
     can_filter_parameter_struct can_filter;
     u8 u8s1, u8s2;
     u16 u16prescaler;

     if(CAN_CalculateCANParameters(cnf->Bps, &u16prescaler, &u8s1, &u8s2))
     {
          return -1;
     }     
     
     rcu_periph_clock_enable(RCU_AF);//复用时钟需要先开启，否则 CAN初始化会失败
     switch(cnf->PinSelect)
     {
          case CAN0_RX_PA11_TX_PA12:
               rcu_periph_clock_enable(RCU_GPIOA);
               gpio_init(GPIOA,GPIO_MODE_IN_FLOATING,GPIO_OSPEED_50MHZ,GPIO_PIN_11);//浮空输入 PA11:Rx
               gpio_init(GPIOA,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_12);//复用推挽输出 PA12:Tx
               gpio_pin_remap_config(GPIO_CAN_FULL_REMAP,ENABLE);  
               break;
          case CAN0_RX_PB8_TX_PB9:
               rcu_periph_clock_enable(RCU_GPIOB);
               gpio_init(GPIOB,GPIO_MODE_IN_FLOATING,GPIO_OSPEED_50MHZ,GPIO_PIN_8);//浮空输入 PB8:Rx
               gpio_init(GPIOB,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_9);//复用推挽输出 PB9:Tx
               gpio_pin_remap_config(GPIO_CAN_PARTIAL_REMAP,ENABLE);  
               break;
          default:
               return -2;
     }

     CAN0_ReceCBF = cnf->ReceCBF;
     
     rcu_periph_clock_enable(RCU_CAN0);         

     /* initialize CAN register */
     can_deinit(CAN0);

     /* initialize CAN */
     can_struct_para_init(CAN_INIT_STRUCT, &can_parameter);
     can_parameter.time_triggered = DISABLE;
     can_parameter.auto_bus_off_recovery = DISABLE;
     can_parameter.auto_wake_up = DISABLE;
     can_parameter.auto_retrans = DISABLE;
     can_parameter.rec_fifo_overwrite = DISABLE;
     can_parameter.trans_fifo_order = DISABLE;
     can_parameter.working_mode = CAN_NORMAL_MODE;
     can_parameter.resync_jump_width = CAN_BT_SJW_1TQ;
//----------------------------   
     //bps = APB1 / (prescaler * (1 + time_segment_1 + time_segment_2))
     //bps = 48 000 000 / (12 * (1 + 12 + 3)) = 250 000
//----------------------------        
     can_parameter.time_segment_1 = (u8s1 - 1);//CAN_BT_BS1_10TQ;//时间段1
     can_parameter.time_segment_2 = (u8s2 - 1);//CAN_BT_BS2_3TQ;//时间段2
     can_parameter.prescaler = u16prescaler;//12;//波特率分频系数

     //APB1=48MHz时，CAN0的波特率为250Kbps,80%采样点
     // can_parameter.time_segment_1 = CAN_BT_BS1_12TQ;
     // can_parameter.time_segment_2 = CAN_BT_BS2_3TQ;
     // can_parameter.prescaler = 12;
     if(can_init(CAN0, &can_parameter) != SUCCESS)
     {
          return -3;
     }
     
//开始接收滤波器的配置     
     can_struct_para_init(CAN_FILTER_STRUCT, &can_filter);
     can_filter.filter_number = 0;
     can_filter.filter_mode = CAN_FILTERMODE_MASK;
     can_filter.filter_bits = CAN_FILTERBITS_32BIT;
     can_filter.filter_list_high = 0x0000U;
     can_filter.filter_list_low = 0x0000U;
     can_filter.filter_mask_high = 0x0000U;
     can_filter.filter_mask_low = 0x0000U;  
     can_filter.filter_fifo_number = CAN_FIFO1;
     can_filter.filter_enable = ENABLE;
     can_filter_init(&can_filter);

     //配置中断
     //nvic_irq_enable(CAN0_RX1_IRQn,0,0);
     NVIC_SetPriority(CAN0_RX1_IRQn, cnf->IntPri);//设置中断优先级
     NVIC_ClearPendingIRQ(CAN0_RX1_IRQn);//清除中断标志
     NVIC_EnableIRQ(CAN0_RX1_IRQn);//使能中断

     //使能接收FIFO1非空中断
     can_interrupt_enable(CAN0, CAN_INT_RFNE1);

     return 0;
}

void CAN0_RX1_IRQHandler(void)
{
//     u8 u8i;
//     can_receive_message_struct g_receive_message;

//      /* initialize receive message */
//     can_struct_para_init(CAN_RX_MESSAGE_STRUCT, &g_receive_message);//每次都初始化？？？还是在配置中只初始化1次？？
//     /* check the receive message */
//     can_message_receive(CAN0, CAN_FIFO1, &g_receive_message);

//      if(CAN0_ReceCBF)
//      {
//           CAN0_ReceCBF();
//      }

//     if(CAN_FF_EXTENDED == g_receive_message.rx_ff)
//     {
//         printf("id = 0x%08X, len = %d, rx_data = ",g_receive_message.rx_efid,g_receive_message.rx_dlen);
//     }
//     else
//     {
//         printf("id = 0x%04X, len = %d, rx_data = ",g_receive_message.rx_sfid,g_receive_message.rx_dlen);
//     }

//     for(u8i = 0;u8i < g_receive_message.rx_dlen;u8i++)
//     {
//         printf("0x%02X ",g_receive_message.rx_data[u8i]);
//     }
//     printf("\r\n");


     u8 u8temp;
     can_receive_message_struct receive_message;
     DRI_CAN_CommunicatDataType cdt;
     //
     can_struct_para_init(CAN_RX_MESSAGE_STRUCT, &receive_message);//初始化
     can_message_receive(CAN0, CAN_FIFO1, &receive_message);
     //
     if(CAN0_ReceCBF != NULL)
     {
          cdt.FrameType = 0;//标准帧
          cdt.FrameID = receive_message.rx_sfid;//帧ID
          if(CAN_FF_EXTENDED == receive_message.rx_ff)
          {//是扩展帧
               cdt.FrameType = 1;//扩展帧
               cdt.FrameID = receive_message.rx_efid;//帧ID
          }
          cdt.FrameDataLen = receive_message.rx_dlen;//帧数据长度
          for(u8temp = 0;u8temp < receive_message.rx_dlen;u8temp++)
          {
               cdt.FrameData[u8temp] = receive_message.rx_data[u8temp];//读取数据
          }
          CAN0_ReceCBF(&cdt);
     }
    
}

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的用户函数代码区------------------------------------                          *
*                                                                                                                                           *
********************************************************************************************************************************************/
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
s8 DRI_CAN0_SendData(DRI_CAN_CommunicatDataType *data)
{
     u8 u8i,transmit_mailbox = 0;
     u32 timeout = 0xFFFF;
     can_trasnmit_message_struct g_transmit_message;

     /* initialize transmit message */
     can_struct_para_init(CAN_TX_MESSAGE_STRUCT, &g_transmit_message);
     g_transmit_message.tx_sfid = data->FrameID;//标准帧ID
     g_transmit_message.tx_efid = data->FrameID;//扩展帧ID
     g_transmit_message.tx_ft = CAN_FT_DATA;//数据帧  CAN_FT_REMOTE//远程帧
     g_transmit_message.tx_ff = CAN_FF_STANDARD;//标准帧格式
     if(data->FrameType)
     {
          g_transmit_message.tx_ff = CAN_FF_EXTENDED;//扩展帧格式
     }
     g_transmit_message.tx_dlen = data->FrameDataLen;//数据长度
     for(u8i = 0;u8i < data->FrameDataLen;u8i++)//数据
     {
          g_transmit_message.tx_data[u8i] = data->FrameData[u8i];
     }
     //---------------------------开始发送 
     transmit_mailbox = can_message_transmit(CAN0, &g_transmit_message);
     if(transmit_mailbox == CAN_NOMAILBOX)//发送数据
     {//启动发送失败
          return -1;
     }
     /* waiting for transmit completed */
     timeout = 0xfFFFF;
     while((CAN_TRANSMIT_OK != can_transmit_states(CAN0, transmit_mailbox)) && (0 != timeout))
     {
          timeout--;
     }
     if(timeout == 0)
     {//发送超时
          return -2;
     }
     return 0;
}


/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的内部函数代码区------------------------------------                          *
*                                                                                                                                           *
********************************************************************************************************************************************/
//计算CAN波特率参数
static s8 CAN_CalculateCANParameters(u32 baudrate,u16 *prescaler,u8 *bs1,u8 *bs2)
{
     u16 u16i,u16temp;
     //CAN时钟源频率为PCLK1
     if(rcu_clock_freq_get(CK_APB1) != 48000000)
     {//时钟源频率不是48M
          return -1;
     }
     u16temp = sizeof(CAN_48M_BpsPara)/sizeof(CAN_48M_BpsPara[0]);
     for(u16i = 0;u16i < u16temp;u16i++)
     {
          if(baudrate == CAN_48M_BpsPara[u16i].bps)
          {
               *prescaler = CAN_48M_BpsPara[u16i].prescaler;
               *bs1 = CAN_48M_BpsPara[u16i].bs1;
               *bs2 = CAN_48M_BpsPara[u16i].bs2;
               return 0;
          }
     }
     return -2;
}

