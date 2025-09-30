/********************************************************************************************************************************************
*                                                                                                                                           *
*              ---------------------------------以下是模块的修改记录区-----------------------------------------                             *
*                                                                                                                                           *
********************************************************************************************************************************************/
/**********************************************
 * 内容：
 * 日期：2022-06-23
 * 作者：YJX
 * 版本号：V1.0（初版）
 ***********************************************
 * 修改内容：
 * 修改日期：
 * 修改作者：
 * 版本号：
 ***********************************************

DMA相关：
【1】、芯片只有一个DMA0，具体详见编程手册P185--9.2
【2】、外设由硬件固定绑定在指定通道上，不能由软件配置，具体详见《编程手册P191--表9-3》



*/

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下模块的说明区-----------------------------------------                                  *
*                                                                                                                                           *
********************************************************************************************************************************************/
/*	

*/
#include "DRI_ComDriver.h"
extern void USBD_LP_IRQHandler(void);
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
#define INTMaxNUM        48  //GD32F103CB最大中断号

static const u32 DMA_DefaultPriority = DMA_PRIORITY_LOW;//DMA_PRIORITY_MEDIUM;//DMA默认优先级
//GD32F103CB的DMA通道采用硬件固定绑定外设。----具体的请参考编程手册P191
static u8 DMA0_ChannelSelectFlag[] = {0,0,0,0,0,0,0}; //DMA0通道选择标志-----初始状态未占用
//本芯片没有DMA1
//static u8 DMA1_ChannelSelectFlag[] = {0,0,0,0,0}; //DMA1通道选择标志---------初始状态未占用

static DRI_ComDriver_DoubleVoidFuncType HardFaultIntterrupt_FCB = NULL;
/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的内部函数申明区------------------------------------                          *
*                                                                                                                                           *
********************************************************************************************************************************************/


/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的系统函数代码区------------------------------------                          *
*                                                                                                                                           *
********************************************************************************************************************************************/
void HardFault_Handler(void)
{
     if(HardFaultIntterrupt_FCB)
     {
          HardFaultIntterrupt_FCB();
     }
}

//DMA通道注册函数
//返回0表示注册成功 -1:表示已被注册 小于-1表示非法通道
s8 DRI_ComDriver_DMAChannelRegedit(dma_channel_enum channel)
{
     switch(channel)
     {
          case DMA_CH0://DMA0通道0
               if(DMA0_ChannelSelectFlag[0])
               {
                    return -1;//已被占用
               }
               DMA0_ChannelSelectFlag[0] = 1;
               return 0;
          case DMA_CH1://DMA0通道1
               if(DMA0_ChannelSelectFlag[1])
               {
                    return -1;//已被占用
               }
               DMA0_ChannelSelectFlag[1] = 1;
               return 0;
          case DMA_CH2://DMA0通道2
               if(DMA0_ChannelSelectFlag[2])
               {
                    return -1;//已被占用
               }
               DMA0_ChannelSelectFlag[2] = 1;
               return 0;
          case DMA_CH3://DMA0通道3
               if(DMA0_ChannelSelectFlag[3])
               {
                    return -1;//已被占用
               }
               DMA0_ChannelSelectFlag[3] = 1;
               return 0;
          case DMA_CH4://DMA0通道4
               if(DMA0_ChannelSelectFlag[4])
               {
                    return -1;//已被占用
               }
               DMA0_ChannelSelectFlag[4] = 1;
               return 0;
          case DMA_CH5://DMA0通道5
               if(DMA0_ChannelSelectFlag[5])
               {
                    return -1;//已被占用
               }
               DMA0_ChannelSelectFlag[5] = 1;
               return 0;
          case DMA_CH6://DMA0通道6
               if(DMA0_ChannelSelectFlag[6])
               {
                    return -1;//已被占用
               }
               DMA0_ChannelSelectFlag[6] = 1;
               return 0;          
          default:
               break;
     }
     return -2;
}

/// @brief DMA通道反注册，将通道释放
/// @param channel 通道号
/// @return 0:反注册成功，小于0:通道号错误
s8 DRI_ComDriver_DMAChannelUnRegedit(dma_channel_enum channel)
{
     switch(channel)
     {
          case DMA_CH0://DMA0通道0
               DMA0_ChannelSelectFlag[0] = 0;
               return 0;
          case DMA_CH1://DMA0通道1
               DMA0_ChannelSelectFlag[1] = 0;
               return 0;
          case DMA_CH2://DMA0通道2
               DMA0_ChannelSelectFlag[2] = 0;
               return 0;
          case DMA_CH3://DMA0通道3
               DMA0_ChannelSelectFlag[3] = 0;
               return 0;
          case DMA_CH4://DMA0通道4
               DMA0_ChannelSelectFlag[4] = 0;
               return 0;
          case DMA_CH5://DMA0通道5
               DMA0_ChannelSelectFlag[5] = 0;
               return 0;
          case DMA_CH6://DMA0通道6
               DMA0_ChannelSelectFlag[6] = 0;
               return 0; 
          default:
               break;
     }
     return -2;
}
/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的用户函数代码区------------------------------------                          *
*                                                                                                                                           *
********************************************************************************************************************************************/

void NULLFP(void)
{
	
}

u8* DRI_ComDriver_OutVersion(void)
{
     return (u8*)"V1.04.250930";
}

//返回芯片Flash空间大小，单位：字节
u32 DRI_ComDriver_OutFlashSize(void)
{
     return (0x0000FFFF & *(u32*)0x1FFFF7E0) * 1024;
}

//返回芯片RAM空间大小，单位：字节
u32 DRI_ComDriver_OutRAMSize(void)
{
     return ((*(u32*)0x1FFFF7E0) >> 16) * 1024;     
}

//uid1:(0~31)、uid2:(32~63)、uid3:(64~95)共96位
void DRI_ComDriver_OutUID(u32 *uid1, u32 *uid2, u32 *uid3)
{
     *uid1 = *(u32*)0x1FFFF7E8;    
     *uid2 = *(u32*)0x1FFFF7EC;
     *uid3 = *(u32*)0x1FFFF7F0;
}

u32 DRI_ComDriver_OutDefaultDMAPriority(void)
{
     return DMA_DefaultPriority;
}

void DRI_ComDriver_Delay1ms(void) 
{
     // 1ms = 1000us，采用双重循环减少计数器占用空间
     vu32 i = 36 * 1000;  // 外层循环次数
     while(i--) 
     {
          __NOP();// 插入空操作平衡流水线
     }
}

void DRI_ComDriver_Delay1us(void) 
{
     // 基于108MHz主频计算：1us需108个时钟周期
     // 每个循环约3个时钟周期（减1+跳转+空操作）
     vu32 i = 36;  // 108 / 3 = 36次循环
     while(i--) 
     {
          __NOP();// 插入空操作平衡流水线
     }
}

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
void DRI_ComDriver_SetHardFaultIntterrupt_FCB(DRI_ComDriver_DoubleVoidFuncType fcbp)
{
     HardFaultIntterrupt_FCB = fcbp;
}

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
void DRI_ComDriver_SetSP(u32 _sp)
{
     __set_MSP(_sp);
}



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
void DRI_ComDriver_DisableAllPeripheralInterrupt(void)
{
     u8 in;
     //关闭所有外设中断(注意，不同芯片INTMaxNUM可能不一样)
     for(in = 0;in <= INTMaxNUM;in++)
     {
          NVIC_DisableIRQ((IRQn_Type)in);
     }
}

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
void DRI_ComDriver_ResetAllPeripheral(void)
{     
     u32 u32dri,u32drn;
     u32 RstEnum[] = {    
          /* APB1 peripherals */
          RCU_TIMER1RST, /*!< TIMER1 clock reset */
          RCU_TIMER2RST, /*!< TIMER2 clock reset */
          RCU_TIMER3RST, /*!< TIMER3 clock reset */
          RCU_TIMER4RST, /*!< TIMER4 clock reset */
          RCU_TIMER5RST, /*!< TIMER5 clock reset */
          RCU_TIMER6RST, /*!< TIMER6 clock reset */
          RCU_WWDGTRST,  /*!< WWDGT clock reset */
          RCU_SPI1RST,   /*!< SPI1 clock reset */
          RCU_SPI2RST,   /*!< SPI2 clock reset */
          RCU_USART1RST, /*!< USART1 clock reset */
          RCU_USART2RST, /*!< USART2 clock reset */
          RCU_UART3RST,  /*!< UART3 clock reset */
          RCU_UART4RST,  /*!< UART4 clock reset */
          RCU_I2C0RST,   /*!< I2C0 clock reset */
          RCU_I2C1RST,   /*!< I2C1 clock reset */
          RCU_USBDRST,   /*!< USBD clock reset */
          RCU_CAN0RST,   /*!< CAN0 clock reset */
          RCU_BKPIRST,   /*!< BKPI clock reset */
          RCU_PMURST,    /*!< PMU clock reset */
          RCU_DACRST,    /*!< DAC clock reset */    
          /* APB2 peripherals */
          RCU_AFRST,     /*!< alternate function clock reset */
          RCU_GPIOARST,  /*!< GPIOA clock reset */
          RCU_GPIOBRST,  /*!< GPIOB clock reset */
          RCU_GPIOCRST,  /*!< GPIOC clock reset */
          RCU_GPIODRST,  /*!< GPIOD clock reset */
          RCU_GPIOERST,  /*!< GPIOE clock reset */
          RCU_GPIOFRST,  /*!< GPIOF clock reset */
          RCU_GPIOGRST,  /*!< GPIOG clock reset */
          RCU_ADC0RST,   /*!< ADC0 clock reset */
          RCU_ADC1RST,   /*!< ADC1 clock reset */
          RCU_TIMER0RST, /*!< TIMER0 clock reset */
          RCU_SPI0RST,   /*!< SPI0 clock reset */
          RCU_TIMER7RST, /*!< TIMER7 clock reset */
          RCU_USART0RST, /*!< USART0 clock reset */         
     };

     //此处将所有外设时钟都恢复到默认值，即关闭所有外设时钟，外设没有时钟，就不会起作用了。
     RCU_AHBEN = 0x00000014;//这个值是手册中的默认值
     RCU_APB2EN = 0x00000000;
     RCU_APB1EN = 0x00000000;

     //所有外设复位
     u32drn = sizeof(RstEnum)/sizeof(RstEnum[0]);
     for(u32dri = 0; u32dri < u32drn; u32dri++)
     {
          rcu_periph_reset_enable((rcu_periph_reset_enum)RstEnum[u32dri]);
          rcu_periph_reset_disable((rcu_periph_reset_enum)RstEnum[u32dri]);
     }
}

//开启总中断C语言写法
/***************************************************************************
* 函 数 名: DRI_ComDriver_EnableAllINT
* 功能描述：使能总中断 函数
           允许所有中断（包括外设和内核中断（如SysTick、PendSV））正常响应。
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
void DRI_ComDriver_EnableAllINT(void)
{
     __set_PRIMASK(0);//开启总中断
     //__enable_irq();
} 

//开启总中断汇编写法
//void __asm DRI_ComDriver_EnableAllINT1(void)
//{
//     CPSIE I //开启总中断
//}

//关闭总中断C语言写法
/***************************************************************************
* 函 数 名: DRI_ComDriver_DisableAllINT
* 功能描述：禁能总中断 函数
           屏蔽所有可屏蔽中断（包括外设中断和内核中断（如SysTick、PendSV）），仅允许NMI（不可屏蔽中断）和系统异常（如HardFault）响应。
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
void DRI_ComDriver_DisableAllINT(void)
{
     __set_PRIMASK(1);//关闭总中断
     //__disable_irq();
}

//关闭总中断汇编写法
//void __asm DRI_ComDriver_DisableAllINT1(void)
//{
//     CPSID I //关闭总中断
//}


//开启总异常C语言写法
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
void DRI_ComDriver_EnableAllFault(void)
{
     __set_FAULTMASK(0);//开启总异常
}

//开启总异常汇编写法
//void __asm DRI_ComDriver_EnableAllFault1(void)
//{
//     CPSIE F //开启总异常
//}

//关闭总异常C语言写法
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
void DRI_ComDriver_DisableAllFault(void)
{
     __set_FAULTMASK(1);//来关闭总异常
}

//关闭总异常汇编写法
//void __asm DRI_ComDriver_DisableAllFault1(void)
//{
//     CPSID F //来关闭总异常
//}



//内核复位_C
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
void DRI_ComDriver_CoreReset(void)
{
     __DSB();
     //置位VECTRESET
     SCB->AIRCR  = ((0x5FA << SCB_AIRCR_VECTKEY_Pos)      |
                    (SCB->AIRCR & SCB_AIRCR_PRIGROUP_Msk) |
                    SCB_AIRCR_VECTRESET_Msk);
     __DSB();
     while(1);
}

//内核复位_汇编
/*
void __asm NVIC_CoreReset_a(void)

{

  LDR R0, =0xE000ED0C

  LDR R1, =0x05FA0001  //置位VECTRESET

  STR R1, [R0]

//;deadloop_Core

//  ;B deadloop_Core

}
*/
//内核主要注意：SCB_AIRCR_VECTRESET_Msk和LDR R1, =0x05FA0001，它是和系统复位唯一的区别。


//系统复位
//C语言版函数：

/***************************************************************************
* 函 数 名: DRI_ComDriver_SysReset
* 功能描述：外设复位 函数
          相当于 NVIC_SystemReset();
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
void DRI_ComDriver_SysReset(void)
{
     //NVIC_SystemReset();

     __DSB();
     SCB->AIRCR  = ((0x5FA << SCB_AIRCR_VECTKEY_Pos)      |
                    (SCB->AIRCR & SCB_AIRCR_PRIGROUP_Msk) |
                    SCB_AIRCR_SYSRESETREQ_Msk);
     __DSB();
     while(1);
}

//USB和CAN0RX0的中断
void USBD_LP_CAN0_RX0_IRQHandler(void)
{
     USBD_LP_IRQHandler(); //USB中断
}
/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的内部函数代码区------------------------------------                          *
*                                                                                                                                           *
********************************************************************************************************************************************/












