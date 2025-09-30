/********************************************************************************************************************************************
*                                                                                                                                           *
*              ---------------------------------以下是模块的修改记录区-----------------------------------------                             *
*                                                                                                                                           *
********************************************************************************************************************************************/
/**********************************************
 * 内容：
 * 日期：2025-09-05
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

#include "DRI_USB.h"
/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下模块的对接函数区-----------------------------------------                                  *
*                                                                                                                                           *
********************************************************************************************************************************************/

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的内部函数申明区------------------------------------                           *
*                                                                                                                                           *
********************************************************************************************************************************************/
static u8 class_init(usb_dev *udev, u8 config_index);
static u8 class_deinit(usb_dev *udev, u8 config_index);
static u8 class_req_proc(usb_dev *udev, usb_req *req);
static u8 class_ctlx_in(usb_dev *udev);                                         /*!< device control IN callback */
static u8 class_ctlx_out(usb_dev *udev);                                        /*!< device control OUT callback */
static void class_data_in(usb_dev *udev, u8 ep_num);
static void class_data_out(usb_dev *udev, u8 ep_num);

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的变量申明和宏定义区--------------------------------                      *
*                                                                                                                                           *
********************************************************************************************************************************************/
#define DEV_IN_EP                            0x81u
#define DEV_OUT_EP                           0x02u
#define DEV_INTIN_EP                         0x83u
#define DEV_MAX_PACKET                       64u
#define DEV_EP0_SIZE                         (DEV_MAX_PACKET)
#define DEV_EPIN_SIZE                        (DEV_MAX_PACKET)
#define DEV_EPOUT_SIZE                       (DEV_MAX_PACKET)
#define DEV_EPINTIN_SIZE                     (DEV_MAX_PACKET)

#define USB_TestFlags                       0//1:表示开启测试 0:表示关闭测试。0x00u

//
static DRI_USB_P0SetupProcessFunc P0SetupProcess;
static DRI_USB_ReceDataFunc ReceDataFunc;
static DRI_USB_P0OutProcessFunc P0OutProcess;
static DRI_USB_PullUpCtrlFunc PullUpCtrlFunc;
// static u8 UsbRxBuffer[PRT_EPOUT_SIZE];//接收缓存
static u8 StopRece = 0;//0表示允许接收 非0表示停止接收
static s8 SendFlag = 1;//0表示发送空闲 非0表示发送忙
static s8 SendIntFlag = 1;//0表示发送空闲 非0表示发送忙

#define UsbEnumDataSendBufferMaxSize 256
static u8 UsbEnumDataSendBuffer[UsbEnumDataSendBufferMaxSize];//USB枚举时数据的发送缓存专用
static u16 UsbEP0OUTDataLen = 0;//USB的EP0OUT数据有效字节数
// //从设备描述符信息中提取
// static u8 MFGStrIndex = 0,MDLStrIndex = 0,SIDStrIndex = 0;//产商、型号(产品)、序列号 的枚举索引号
// //从配置描述符信息中提取
// static u8 CNFStrIndex = 0,INFStrIndex = 0;//配置字符、接口字符 的枚举索引号
// //
// static u8 StrNumber = 0;//枚举时字符串信息的个数
// //
// static u8 dev_prt_altset = 0u;

static const usb_desc_ep EP_IN_Data = 
{
     .header.bLength = 0x07,    ///本描述段长度，单位字节
     .header.bDescriptorType = 0x05,    ///描述符类型: 端点描述符
     .bEndpointAddress = DEV_IN_EP,    ///端点方向和端点号: 最高位为方向(主机角度)(1:IN方向，即设备到主机  0:OUT方向，即主机到设备)  低4位为端点号(本配置值:IN方向,端点1)
     .bmAttributes = 0x02,    ///属性: 传输类型: 批量传输    0:为控制传输类型  1:为同步传输类型  2:为批量传输类型  3:为中断传输类型
     .wMaxPacketSize = DEV_EPIN_SIZE,    ///最大包大小
     .bInterval = 0x00    ///传输间隔: 0表示没有间隔
};

static const usb_desc_ep EP_OUT_Data = 
{
     .header.bLength = 0x07,    ///本描述段长度，单位字节
     .header.bDescriptorType = 0x05,    ///描述符类型: 端点描述符
     .bEndpointAddress = DEV_OUT_EP,    ///端点方向和端点号: 最高位为方向(主机角度)(1:IN方向，即设备到主机  0:OUT方向，即主机到设备)  低4位为端点号(本配置值:IN方向,端点1)
     .bmAttributes = 0x02,    ///属性: 传输类型: 批量传输    0:为控制传输类型  1:为同步传输类型  2:为批量传输类型  3:为中断传输类型
     .wMaxPacketSize = DEV_EPOUT_SIZE,    ///最大包大小
     .bInterval = 0x00    ///传输间隔: 0表示没有间隔
};

static const usb_desc_ep EP_INT_Data = 
{
     .header.bLength = 0x07,    ///本描述段长度，单位字节
     .header.bDescriptorType = 0x05,    ///描述符类型: 端点描述符
     .bEndpointAddress = DEV_INTIN_EP,    ///端点方向和端点号: 最高位为方向(主机角度)(1:IN方向，即设备到主机  0:OUT方向，即主机到设备)  低4位为端点号(本配置值:IN方向,端点1)
     .bmAttributes = 0x03,    ///属性: 传输类型: 批量传输    0:为控制传输类型  1:为同步传输类型  2:为批量传输类型  3:为中断传输类型
     .wMaxPacketSize = DEV_EPINTIN_SIZE,    ///最大包大小
     .bInterval = 0xff    ///传输间隔:
};

static usb_dev usb_device;
static u8 usb_recedata_buf[DEV_MAX_PACKET];

static usb_desc usbd_desc_cb = 
{//描述数据
     .dev_desc    = (u8 *)0,
     .config_desc = (u8 *)0,
     .bos_desc    = (u8 *)0,
     .strings     = 0
};

static usb_class usbd_class_cb = 
{//类回调函数
     .req_cmd            = 0xFFU,
     .req_altset         = 0U,
     //
     .init               = class_init,
     .deinit             = class_deinit,
     .req_process        = class_req_proc,
     .ctlx_in            = class_ctlx_in,
     .ctlx_out           = class_ctlx_out,
     .data_in            = class_data_in,
     .data_out           = class_data_out,
};

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的系统函数代码区------------------------------------                          *
*                                                                                                                                           *
********************************************************************************************************************************************/
/***************************************************************************
* 函 数 名: DRI_USB_Config
* 功能描述：USB配置函数
* 入口参数：
            DRI_USBCnfType *cnfp:USB接口配置参数结构体指针
* 出口参数：
            -1:表示配置失败
            0:表示配置成功
* 注意事项:  
            无
* 例     如:
* 修改记录 :
          
*           2025-09-05 BY:YJX
***************************************************************************/
s8 DRI_USB_Config(DRI_USBCnfType *cnfp)
{//此处要增加确认主频是否为96MHz，否则在8MHz的外部晶振下，USB需要的48MHz时钟无法精确产生
     u32 system_clock;

     // /* enable USB pull-up pin clock */ 
     //由于芯片没有内置的DP+上拉电阻，需要外部上拉控制。
     //rcu_periph_clock_enable(RCU_PULLUP);
     //gpio_init(USB_PULLUP, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, USB_PULLUP_PIN);
     //
     // rcu_periph_clock_enable(RCU_GPIOA);
     // gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_11);
     // gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_12);


     system_clock = rcu_clock_freq_get(CK_SYS);
     if (48000000U == system_clock) 
     {
          rcu_usb_clock_config(RCU_CKUSB_CKPLL_DIV1);
     } 
     else if (72000000U == system_clock) 
     {
          rcu_usb_clock_config(RCU_CKUSB_CKPLL_DIV1_5);
     } 
     else if (96000000U == system_clock) 
     {
          rcu_usb_clock_config(RCU_CKUSB_CKPLL_DIV2);
     } 
     else 
     {
          return -1;
     }

     /* enable USB APB1 clock */
     rcu_periph_clock_enable(RCU_USBD);

//---------------- 初始化 ----------------------
     P0SetupProcess = cnfp->P0SetupProcess;
     ReceDataFunc = cnfp->ReceDataFunc;
     P0OutProcess = cnfp->P0OutProcess;
     PullUpCtrlFunc = cnfp->PullUpFunc;
//---------------------
     gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_11 | GPIO_PIN_12); //PA11/PA12
     //usbd_disconnect(&usb_device);

     /* USB device configuration */
     usbd_init(&usb_device, &usbd_desc_cb, &usbd_class_cb);

     /* NVIC configuration */
     /* 2 bits for pre-emption priority, 2 bits for subpriority */
     // nvic_vector_table_set(NVIC_VECTTAB_FLASH, 0x00U);
     // nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
     // nvic_irq_enable(USBD_LP_CAN0_RX0_IRQn, 2U, 0U);
     NVIC_SetPriority(USBD_LP_CAN0_RX0_IRQn, cnfp->IntPri);//设置中断优先级
     NVIC_ClearPendingIRQ(USBD_LP_CAN0_RX0_IRQn);//清除中断标志
     NVIC_EnableIRQ(USBD_LP_CAN0_RX0_IRQn);//使能中断

     /* enabled USB pull-up */
     usbd_connect(&usb_device);
     
     //while (USBD_CONFIGURED != usb_device.cur_status) 
     //{
     //   /* wait for standard USB enumeration is finished */
     //}
     return 0;
}

//----------------------------------------------------------------------------------------
void usb_udelay(const u32 usec)
{
    u32 un = usec;
    do{
          DRI_ComDriver_Delay1us();
     }while(--un);
}

void usb_mdelay(const u32 msec)
{
    u32 mn = msec;
    do{
          DRI_ComDriver_Delay1ms();
     }while(--mn);
}
/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的用户函数代码区------------------------------------                          *
*                                                                                                                                           *
********************************************************************************************************************************************/
/***************************************************************************
* 函 数 名: DRI_USB_Disconnect
* 功能描述：USB软断开函数
* 入口参数：
            
* 出口参数：
            无
* 注意事项:  
            无
* 例     如:
* 修改记录 :
          
*           2025-09-05 BY:YJX
***************************************************************************/
void DRI_USB_Disconnect(void)
{
     usbd_disconnect(&usb_device);
}

/***************************************************************************
* 函 数 名: DRI_USB_Connect
* 功能描述：USB软接入函数
* 入口参数：
            
* 出口参数：
            无
* 注意事项:  
            无
* 例     如:
* 修改记录 :
          
*           2025-09-05 BY:YJX
***************************************************************************/
void DRI_USB_Connect(void)
{
     usbd_connect(&usb_device);
}

/***************************************************************************
* 函 数 名: DRI_USB_ReConnect
* 功能描述：阻塞型 USB重拔插函数
* 入口参数：
            无
* 出口参数：
            无
* 注意事项:  
            无
* 例     如:
* 修改记录 :
          
*           2025-09-05 BY:YJX
***************************************************************************/
void DRI_USB_ReConnect(void)
{//在阻塞状态下，此操作好像会出问题，待排查！！！
     usbd_disconnect(&usb_device);
     usb_mdelay(300);
     usbd_connect(&usb_device);
}

/***************************************************************************
* 函 数 名: DRI_USB_EnableReceive
* 功能描述：使能接收函数
* 入口参数：
            无
* 出口参数：
            无
* 注意事项:  
            无
* 例     如:
* 修改记录 :
*           2025-09-05 BY:YJX
***************************************************************************/
void DRI_USB_EnableReceive(void)
{
     if(StopRece)
     {
          StopRece = 0; 
          //hd_usb_devepstatusset(&usb_dev,PRT_OUT_EP,USB_EP_RX_VALID);
          //printf("E\r\n");
          usbd_ep_recev(&usb_device, DEV_OUT_EP, usb_recedata_buf, DEV_EPOUT_SIZE);
     }     
}

/***************************************************************************
* 函 数 名: DRI_USB_DisableReceive
* 功能描述：禁止接收函数
* 入口参数：
            无
* 出口参数：
            无
* 注意事项:  
            无
* 例     如:
* 修改记录 :
*           2025-09-05 BY:YJX
***************************************************************************/
void DRI_USB_DisableReceive(void)
{
     StopRece = 1;
     //printf("D\r\n");
     //hd_usb_devepstatusset(&usb_dev,PRT_OUT_EP,USB_EP_RX_NAK);
}

/***************************************************************************
* 函 数 名: DRI_USB_SendDataNonZPL
* 功能描述：阻塞型发送数据(有ZPL操作) 函数
* 入口参数：
            u8 *sd:待发数据首地址
            u16 sl:待发字节数
* 出口参数：
            返回值:小于0:发送失败
                  其他值:已发送字节数
* 注意事项:  
            不支持在接收回调中发送数据!!!
* 例     如:
* 修改记录 :

*           2025-09-05 BY:YJX
***************************************************************************/
s32 DRI_USB_SendDataZPL(u8 *sd,u16 sl)
{
     u32 u32count,u32time = 20000000;

     SendFlag = 1;
     usbd_ep_send(&usb_device,DEV_IN_EP,sd,sl); 

     if(((sl % DEV_EPIN_SIZE) == 0) && sl)
     {//是整数倍时
          u32count = 0;
          while((u32count++ < u32time) && (SendFlag));//等待发送完成

          SendFlag = 1;
          usbd_ep_send(&usb_device,DEV_IN_EP,NULL,0);//增加发送一个0字节的操作             
     }

     u32count = 0;
     while((u32count++ < u32time) && (SendFlag));//等待发送完成
     
     return sl;
}

/***************************************************************************
* 函 数 名: DRI_USB_SendData
* 功能描述：阻塞型发送数据 函数
* 入口参数：
            u8 *sd:待发数据首地址
            u16 sl:待发字节数
* 出口参数：
            返回值:小于0:发送失败
                  其他值:已发送字节数
* 注意事项:  
            不支持在接收回调中发送数据!!!
            本函数无ZPL操作，当sl为端点最大包长的整数倍时，会发送不成功！！！
            需要上层根据情况再次调用本函数发送ZPL操作(ZPL操作即sl为0)
            【1】、CDC类、Print类，当sl为端点最大包长的整数倍时，需要紧接调用本函数来发送ZPL操作，不然在整数倍时，会发送不成功！！！非整数倍时，无需ZPL操作
            【2】、Storage类无需ZPL操作，不然会异常的！！
* 例     如:
* 修改记录 :

*           2025-09-05 BY:YJX
***************************************************************************/
s32 DRI_USB_SendData(u8 *sd,u16 sl)
{
     u32 u32count,u32time = 20000000;

     SendFlag = 1;
     usbd_ep_send(&usb_device,DEV_IN_EP,sd,sl); 

     u32count = 0;
     while((u32count++ < u32time) && (SendFlag));//等待发送完成
     
     return sl;
}


/***************************************************************************
* 函 数 名: DRI_USB_NonBlockSendData
* 功能描述：非阻塞型启动发送数据 函数
* 入口参数：
            u8 *sd:待发数据首地址
            u8 sl:待发字节数
* 出口参数：
            小于0:发送忙 或 失败
           其他值:已启动发送的字节数
* 注意事项:  
            不支持在接收回调中发送数据!!!
            本函数无ZPL操作，当sl为端点最大包长的整数倍时，会发送不成功！！！
            需要上层根据情况再次调用本函数发送ZPL操作(ZPL操作即sl为0)
            【1】、CDC类、Print类，当sl为端点最大包长的整数倍时，需要紧接调用本函数来发送ZPL操作，不然在整数倍时，会发送不成功！！！非整数倍时，无需ZPL操作
            【2】、Storage类无需ZPL操作，不然会异常的！！

            需配合DRI_USB_CheckNonBlockSendState函数来判断是否发送完成
* 例     如:
* 修改记录:
* 日期        版本        作者         说明
*           2025-09-05 BY:YJX
***************************************************************************/
s32 DRI_USB_NonBlockSendData(u8 *sd,u16 sl)
{
     if(SendFlag)
     {//未发送完成
          return -1;
     }    

     SendFlag = 1;
     usbd_ep_send(&usb_device,DEV_IN_EP,sd,sl); 
     
     return sl;
}

/***************************************************************************
* 函 数 名: DRI_USB_CheckNonBlockSendState
* 功能描述：判断非阻塞型发送状态 函数
* 入口参数：
            无
* 出口参数：
            0：发送完成
          非0：发送未完成
* 注意事项:  
            
* 例     如:
* 修改记录 :
*           2025-09-05 BY:YJX
***************************************************************************/
s8 DRI_USB_CheckNonBlockSendState(void)
{
     if(SendFlag)
     {
          return -1;
     }
     return 0;
}

/***************************************************************************
* 函 数 名: DRI_USB_SendData_INT
* 功能描述：阻塞型发送INT数据 函数
* 入口参数：
            u8 *sd:待发数据首地址
            u16 sl:待发字节数
* 出口参数：
            返回值:小于0:发送失败
                  其他值:已发送字节数
* 注意事项:  
            不支持在接收回调中发送数据!!!
* 例     如:
* 修改记录 :
*           2025-09-05 BY:YJX
***************************************************************************/
s32 DRI_USB_SendData_INT(u8 *sd,u16 sl)
{     
     return -2;//目前INT数据发送时会卡住，暂时关闭
}

/***************************************************************************
* 函 数 名: DRI_USB_NonBlockSendData_INT
* 功能描述：非阻塞型启动发送INT数据 函数
* 入口参数：
            u8 *sd:待发数据首地址
            u8 sl:待发字节数
* 出口参数：
            小于0:发送忙 或 失败
           其他值:已启动发送的字节数
* 注意事项:  
            不支持在接收回调中发送数据!!!
            需配合DRI_USB_CheckNonBlockSendINTState函数来判断是否发送完成
* 例     如:
* 修改记录:
*           2025-09-05 BY:YJX
***************************************************************************/
s32 DRI_USB_NonBlockSendData_INT(u8 *sd,u16 sl)
{
     return -2;//目前INT数据发送时会卡住，暂时关闭
}

/***************************************************************************
* 函 数 名: DRI_USB_CheckNonBlockSendINTState
* 功能描述：判断非阻塞型发送INT状态 函数
* 入口参数：
            无
* 出口参数：
            0：发送完成
          非0：发送未完成
* 注意事项:  
            
* 例     如:
* 修改记录 : 
*           2025-09-05 BY:YJX
***************************************************************************/
s8 DRI_USB_CheckNonBlockSendINTState(void)
{
     if(SendIntFlag)
     {
          return -1;
     }
     return 0;
}

/***************************************************************************
* 函 数 名: DRI_USB_OutEndPointInfo
* 功能描述：输出端点信息 函数
* 入口参数：
            DRI_USB_EPType *ep:存放端点信息的结构体指针
* 出口参数：
            无
* 注意事项:  
            
* 例     如:
* 修改记录 :
*           2025-09-05 BY:YJX
***************************************************************************/
void DRI_USB_OutEndPointInfo(DRI_USB_EPType *ep)
{
     
     ep->InEP_Num = (EP_IN_Data.bEndpointAddress & 0x7F);
     ep->InEP_Type = EP_IN_Data.bmAttributes;
     ep->InEP_MaxPacketSize = EP_IN_Data.wMaxPacketSize;
     //
     ep->OutEP_Num = (EP_OUT_Data.bEndpointAddress & 0x7F);
     ep->OutEP_Type = EP_OUT_Data.bmAttributes;
     ep->OutEP_MaxPacketSize = EP_OUT_Data.wMaxPacketSize;
     //
     ep->IntInEP_Num = (EP_INT_Data.bEndpointAddress & 0x7F);
     ep->IntInEP_Type = EP_INT_Data.bmAttributes;
     ep->IntInEP_MaxPacketSize = EP_INT_Data.wMaxPacketSize;
     //
     ep->P0EP_MaxPacketSize = DEV_EP0_SIZE;
}

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的内部函数代码区------------------------------------                          *
*                                                                                                                                           *
********************************************************************************************************************************************/
void USBD_LP_IRQHandler(void)
{     
     usbd_isr();
}

//uf = 0:断开 =非0:连接
void USBD_PullUp(u8 uf)
{
     if(PullUpCtrlFunc)
     {
          PullUpCtrlFunc(uf);
     }     
}

//----------------------------------------------------------------------------------------
//获取【设备描述符】数据
//返回【设备描述符】数据指针，u16 *len为返回【设备描述符】数据字节数
u8 *DRI_USB_dev_desc_get(usb_dev *udev, u8 index, u16 *len)
{
#if USB_TestFlags != 0
     (void)index;
     *len = udev->dev.desc->dev_desc[0];
     return udev->dev.desc->dev_desc;
#else
     s8 s8temp;
     u16 u16temp;
     //此处生成一个请求设备描述符的标准命令
     u8 UsbCmd[] = {0x80,//方向为IN，标准命令
                    0x06,//获取描述符命令
                    0x00,0x01,//wValue-->设备描述符
                    0x00,0x00,//wIndex
                    0xff,0xff};//wLength
     //printf("设备描述符:\r\n");
     *len = 0;
     u16temp = UsbEnumDataSendBufferMaxSize;
     s8temp = P0SetupProcess(UsbCmd,sizeof(UsbCmd),UsbEnumDataSendBuffer,&u16temp);
     if(s8temp == 1)
     {//有待发送的数据
          *len = u16temp;  
     }
     return UsbEnumDataSendBuffer;
#endif
}

//获取【配置描述符】数据
//返回【配置描述符】数据指针，u16 *len为返回【配置描述符】数据字节数
u8 *DRI_USB_config_desc_get(usb_dev *udev, u8 index, u16 *len)
{
#if USB_TestFlags != 0     
     (void)index;
     *len = udev->dev.desc->config_desc[2] | (udev->dev.desc->config_desc[3] << 8);
     return udev->dev.desc->config_desc;
#else
     s8 s8temp;
     u16 u16temp;
     //此处生成一个请求设备描述符的标准命令
     u8 UsbCmd[] = {0x80,//方向为IN，标准命令
                    0x06,//获取描述符命令
                    0x00,0x02,//wValue-->配置描述符
                    0x00,0x00,//wIndex
                    0xff,0xff};//wLength
     *len = 0;
     //printf("configstr:\r\n");
     u16temp = UsbEnumDataSendBufferMaxSize;
     s8temp = P0SetupProcess(UsbCmd,sizeof(UsbCmd),UsbEnumDataSendBuffer,&u16temp);     
     if(s8temp == 1)
     {//有待发送的数据
          //此处的数据是字符串描述符的配置信息  
          *len = u16temp;  
     }
     
     return UsbEnumDataSendBuffer;
#endif     
}

//获取【字符串】数据
//返回【字符串】数据指针，u8 index为字符串索引号 u16 *len为返回【字符串】数据字节数
u8 *DRI_USB_str_desc_get(usb_dev *udev, u8 index, u16 *len)
{
#if USB_TestFlags != 0
     u8 *desc = udev->dev.desc->strings[index];
     *len = desc[0];
     return desc;
#else
     s8 s8temp;
     u16 u16temp;
     //此处生成一个请求设备描述符的标准命令
     u8 UsbCmd[] = {0x80,//方向为IN，标准命令
                    0x06,//获取描述符命令
                    index,0x03,//wValue-->0x00语言编码,0x03字符串描述符
                    0x00,0x00,//wIndex
                    0x04,0x00};//wLength
     if(index)
     {
          UsbCmd[6] = 0xff;
          UsbCmd[7] = 0xff;
     }
     *len = 0;
     //printf("langiddesc:\r\n");
     u16temp = UsbEnumDataSendBufferMaxSize;
     s8temp = P0SetupProcess(UsbCmd,sizeof(UsbCmd),UsbEnumDataSendBuffer,&u16temp);     
     if(s8temp == 1)
     {//有待发送的数据
          //此处的数据是字符串描述符的语言编码          
          *len = u16temp; 
     }
     
     return UsbEnumDataSendBuffer;
#endif     
}

u8 *DRI_USB_bos_desc_get(usb_dev *udev, u8 index, u16 *len)
{
#if USB_TestFlags != 0
     (void)index;
     *len = udev->dev.desc->bos_desc[2];
     return udev->dev.desc->bos_desc;
#else
     *len = 0;
     return (u8*)0;
#endif
}


//-----------------------------------------------------------------------------------
static u8 class_init(usb_dev *udev, u8 config_index)
{
     /* initialize the data TX/RX endpoint */
     usbd_ep_init(udev, EP_BUF_SNG, BULK_TX_ADDR, &EP_IN_Data);
     usbd_ep_init(udev, EP_BUF_SNG, BULK_RX_ADDR, &EP_OUT_Data);
     usbd_ep_init(udev, EP_BUF_SNG, INT_IN_ADDR, &EP_INT_Data);

     udev->ep_transc[DEV_IN_EP & 0x7F][TRANSC_IN] = usbd_class_cb.data_in;
     udev->ep_transc[DEV_OUT_EP & 0x7F][TRANSC_OUT] = usbd_class_cb.data_out;


     /* prepare to receive data */
     usbd_ep_recev(udev, DEV_OUT_EP, usb_recedata_buf, DEV_EPOUT_SIZE);

     SendFlag = 0;//发送空闲
     SendIntFlag = 0;//发送中断空闲
     StopRece = 0;//允许接收

     return USBD_OK;
}

static u8 class_deinit(usb_dev *udev, u8 config_index)
{
     usbd_ep_deinit(udev, DEV_IN_EP);
     usbd_ep_deinit(udev, DEV_OUT_EP);
     usbd_ep_deinit(udev, DEV_INTIN_EP);

     SendFlag = 0;//发送空闲
     SendIntFlag = 0;//发送中断空闲
     StopRece = 0;//允许接收
    return USBD_OK;
}

static u8 class_req_proc(usb_dev *udev, usb_req *req)
{
     usb_transc *transc = &udev->transc_in[0];
     s8 s8temp;
     u16 u16temp;
     //此处生成一个标准命令
     u8 UsbCmd[] = {req->bmRequestType,//
                    req->bRequest,//
                    (u8)(req->wValue),(u8)((req->wValue)>>8),//wValue
                    (u8)(req->wIndex),(u8)((req->wIndex)>>8),//wIndex
                    (u8)(req->wLength),(u8)((req->wLength)>>8)};//wLength
     //printf("Setup\r\n");//
     u16temp = UsbEnumDataSendBufferMaxSize;
     s8temp = P0SetupProcess(UsbCmd,sizeof(UsbCmd),UsbEnumDataSendBuffer,&u16temp); 
     if(s8temp > 0)
     {//已处理
          if((u16temp) && (s8temp == 1))
          {//有待发送数据
               transc->xfer_buf = UsbEnumDataSendBuffer;
               transc->xfer_len = u16temp;
          }
          else if((u16temp) && (s8temp == 2))
          {//EP0有待接收数据               
               UsbEP0OUTDataLen = u16temp;
               if(UsbEP0OUTDataLen > UsbEnumDataSendBufferMaxSize)
               {//防止越界
                    UsbEP0OUTDataLen = UsbEnumDataSendBufferMaxSize;
               }
               //使能EP0准备接收命令数据包
               //usbd_class_cb.alter_set = req->bRequest;
               transc = &udev->transc_out[0];//设置传输结构体指针
               transc->xfer_len = UsbEP0OUTDataLen;//req->wLength;//设置传输长度
               transc->xfer_buf = UsbEnumDataSendBuffer;//设置传输缓冲区
          }          
     }
     return USBD_OK;    
}

//控制端点输入数据
static u8 class_ctlx_in(usb_dev *udev)
{
     return USBD_OK;
}

//控制端点输出数据
static u8 class_ctlx_out(usb_dev *udev)
{//EP0收到数据后的回调函数
     if((P0OutProcess))// && (usbd_class_cb.alter_set != 0xFFU))
     {
          //usbd_class_cb.alter_set = 0xFFU;
          P0OutProcess(UsbEnumDataSendBuffer,UsbEP0OUTDataLen);
     }
     return USBD_OK;
}

static void class_data_in(usb_dev *udev, u8 ep_num)
{//数据发送完成的回调函数
     SendFlag = 0;    
     SendIntFlag = 0;
}

static void class_data_out(usb_dev *udev, u8 ep_num)
{//收到数据后的回调函数
     //u32 u32i;
     if(ep_num == DEV_OUT_EP)
     {
          u8 rdata[128]; 
          u32 rl = ((usb_dev *)udev)->transc_out[ep_num].xfer_count;
          u32 u32temp;
          if(rl)
          {               
               if(rl > 128)
               {
                    rl = 128;//防止溢出越界
               }
               for(u32temp = 0;u32temp < rl;u32temp++)
               {
                    rdata[u32temp] = usb_recedata_buf[u32temp];
               }
          }

          if(StopRece)
          {//如果停止接收，则置端口要响应NAK----下一帧马上就收不进来了
               //hd_usb_devepstatusset(pdev,PRT_OUT_EP,USB_EP_RX_NAK);
               //printf("停止接收\r\n");
          }
          else
          {
               usbd_ep_recev(udev, DEV_OUT_EP, usb_recedata_buf, DEV_EPOUT_SIZE);
          }

          if(rl)
          {
               ReceDataFunc(rdata,rl);//
          }         
     }
}


