/********************************************************************************************************************************************
*                                                                                                                                           *
*              ---------------------------------以下是模块的修改记录区-----------------------------------------                             *
*                                                                                                                                           *
********************************************************************************************************************************************/
/**********************************************
 * 内容：
 * 日期：2025-09-29
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

SPI外设相关：
     【2】、根据《数据手册 P86》4.17 -----> 时钟源：芯片SPI频率最大为27MHz。
     【3】、根据《编程手册 P128》5.6.7 ---> SPI0--时钟源取自于 APB2
     【4】、根据《编程手册 P129》5.6.8 ---> SPI1--时钟源取自于 APB1

使用例子：
*/
#include "DRI_SPIn.h"
#include "DRI_GPIO.h"
/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下模块的对接函数区-----------------------------------------                                  *
*                                                                                                                                           *
********************************************************************************************************************************************/
#define SPIn_GPIO_Config(PNA,PN)   DRI_GPIO_Config(PNA,PN,GDOut)
#define SPIn_GPIO_OUT_1(PNA,PN)    DRI_GPIO_OUT_1(PNA,PN)
#define SPIn_GPIO_OUT_0(PNA,PN)    DRI_GPIO_OUT_0(PNA,PN)
/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的变量申明和宏定义区--------------------------------                      *
*                                                                                                                                           *
********************************************************************************************************************************************/
#define SPI_MAX_CLK           27000000//芯片决定最大的时钟频率
//
static u8 Spi0_Init_Flag = 0,Spi1_Init_Flag = 0;
static PortNum Spi0_Cs_PortNum,Spi1_Cs_PortNum;
static PinNum Spi0_Cs_PinNum,Spi1_Cs_PinNum;
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
s8 DRI_SPIn_Config(DRI_SPICnfType *cnfp)
{
     u8 *spi_init_flag_p;
     u32 spi_periph,spi_freq;
     spi_parameter_struct spi_init_struct;
     PortNum *spi_cs_portnum_p;
     PinNum *spi_cs_pinnum_p;

     if((cnfp->speed == 0) || (cnfp->speed > SPI_MAX_CLK))
     {
          return -1;
     }

     rcu_periph_clock_enable(RCU_AF);//使能复用功能时钟
     spi_struct_para_init(&spi_init_struct);
     //
     switch(cnfp->PinSelect)
     {
          case SPI0_SCK_PA5_MISO_PA6_MOSI_PA7://----->Altemate
               spi_init_flag_p = &Spi0_Init_Flag;
               spi_cs_portnum_p = &Spi0_Cs_PortNum;
               spi_cs_pinnum_p = &Spi0_Cs_PinNum;
               rcu_periph_clock_enable(RCU_GPIOA);               
               rcu_periph_clock_enable(RCU_SPI0);
               gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5 | GPIO_PIN_7);
               gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_6);
               spi_periph = SPI0;
               spi_freq = rcu_clock_freq_get(CK_APB2);
               break;
          case SPI0_SCK_PB3_MISO_PB4_MOSI_PB5://----->Remap
               spi_init_flag_p = &Spi0_Init_Flag;
               spi_cs_portnum_p = &Spi0_Cs_PortNum;
               spi_cs_pinnum_p = &Spi0_Cs_PinNum;
               rcu_periph_clock_enable(RCU_GPIOB);               
               rcu_periph_clock_enable(RCU_SPI0);
               gpio_pin_remap_config(GPIO_SPI0_REMAP, ENABLE);
               gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3 | GPIO_PIN_5);
               gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_4);
               spi_periph = SPI0;
               spi_freq = rcu_clock_freq_get(CK_APB2);
               break;
          case SPI1_SCK_PB13_MISO_PB14_MOSI_PB15://----->Altemate
               spi_init_flag_p = &Spi1_Init_Flag;
               spi_cs_portnum_p = &Spi1_Cs_PortNum;
               spi_cs_pinnum_p = &Spi1_Cs_PinNum;
               rcu_periph_clock_enable(RCU_GPIOB);               
               rcu_periph_clock_enable(RCU_SPI1);
               gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13 | GPIO_PIN_15);
               gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_14);
               spi_periph = SPI1;
               spi_freq = rcu_clock_freq_get(CK_APB1);
               break;
          default:
               return -2;
     }

     //
     *spi_init_flag_p = 0;

     //配置片先
     if(SPIn_GPIO_Config(cnfp->CSport,cnfp->CSpin))
     {
          return -3;
     }
     *spi_cs_portnum_p = cnfp->CSport;
     *spi_cs_pinnum_p = cnfp->CSpin;
     if(cnfp->CSDefaultLevel)
     {
          SPIn_GPIO_OUT_1(*spi_cs_portnum_p,*spi_cs_pinnum_p);
     }
     else
     {
          SPIn_GPIO_OUT_0(*spi_cs_portnum_p,*spi_cs_pinnum_p);
     }

     //配置SPI参数
     switch(cnfp->WorkMode)
     {
          case SPIMode0:
               spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;
               break;
          case SPIMode1:
               spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_2EDGE;
               break;
          case SPIMode2:
               spi_init_struct.clock_polarity_phase = SPI_CK_PL_HIGH_PH_1EDGE;
               break;
          case SPIMode3:
               spi_init_struct.clock_polarity_phase = SPI_CK_PL_HIGH_PH_2EDGE;
               break;
          default:
               return -3;
     }
     //SPI1---->APB1
     //SPI0---->APB2
     if((spi_freq >> 1) <= cnfp->speed)
     {
          spi_init_struct.prescale = SPI_PSC_2;
     }
     else if((spi_freq >> 2) <= cnfp->speed)
     {
          spi_init_struct.prescale = SPI_PSC_4;
     }
     else if((spi_freq >> 3) <= cnfp->speed)
     {
          spi_init_struct.prescale = SPI_PSC_8;
     }
     else if((spi_freq >> 4) <= cnfp->speed)
     {
          spi_init_struct.prescale = SPI_PSC_16;
     }
     else if((spi_freq >> 5) <= cnfp->speed)
     {
          spi_init_struct.prescale = SPI_PSC_32;
     }
     else if((spi_freq >> 6) <= cnfp->speed)
     {
          spi_init_struct.prescale = SPI_PSC_64;
     }
     else if((spi_freq >> 7) <= cnfp->speed)
     {
          spi_init_struct.prescale = SPI_PSC_128;
     }
     else if((spi_freq >> 8) <= cnfp->speed)
     {
          spi_init_struct.prescale = SPI_PSC_256;
     }
     else
     {
          return -4;
     }
     //
     spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;//全双工
     spi_init_struct.device_mode          = SPI_MASTER;//主机模式
     spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;//8位数据格式
     spi_init_struct.nss                  = SPI_NSS_SOFT;//软件控制NSS
     spi_init_struct.endian               = SPI_ENDIAN_MSB;//高位在前   

     /* deinitialize SPI and the parameters */
     spi_i2s_deinit(spi_periph); 
     spi_init(spi_periph, &spi_init_struct);
     spi_enable(spi_periph);
          
     //
     *spi_init_flag_p = 1;

     return 0;
}



/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的用户函数代码区------------------------------------                          *
*                                                                                                                                           *
********************************************************************************************************************************************/
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
u8 DRI_SPI0_ReadWriteByte(u8 u8data)
{                           
     u8 u8result = 0;
     if(Spi0_Init_Flag)
     {//SPI0已初始化
          while(RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_TBE));//等待发送缓存空
          spi_i2s_data_transmit(SPI0, (u16)u8data); //发送数据
          while(RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_RBNE));//等待接收缓存满
          u8result = (u8)spi_i2s_data_receive(SPI0); //接收数据
     }
     return u8result;
}

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
void DRI_SPI0_CS(u8 cs)
{
     if(Spi0_Init_Flag)
     {//SPI0已初始化
          if(cs)
          {
               SPIn_GPIO_OUT_1(Spi0_Cs_PortNum, Spi0_Cs_PinNum);
          }
          else
          {
               SPIn_GPIO_OUT_0(Spi0_Cs_PortNum, Spi0_Cs_PinNum);
          }
     }
}

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
u8 DRI_SPI1_ReadWriteByte(u8 u8data)
{
     u8 u8result = 0;
     if(Spi1_Init_Flag)
     {//SPI1已初始化
          while(RESET == spi_i2s_flag_get(SPI1, SPI_FLAG_TBE));//等待发送缓存空
          spi_i2s_data_transmit(SPI1, (u16)u8data); //发送数据
          while(RESET == spi_i2s_flag_get(SPI1, SPI_FLAG_RBNE));//等待接收缓存满
          u8result = (u8)spi_i2s_data_receive(SPI1); //接收数据
     }
     return u8result;
}

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
void DRI_SPI1_CS(u8 cs)
{
     if(Spi1_Init_Flag)
     {//SPI1已初始化
          if(cs)
          {
               SPIn_GPIO_OUT_1(Spi1_Cs_PortNum, Spi1_Cs_PinNum);
          }
          else
          {
               SPIn_GPIO_OUT_0(Spi1_Cs_PortNum, Spi1_Cs_PinNum);
          }
     }
}
/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的内部函数代码区------------------------------------                          *
*                                                                                                                                           *
********************************************************************************************************************************************/










