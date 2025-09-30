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
     【1】、目前暂时固定使用DMA模式进行。

ADC外设相关：
     【1】、根据《编程手册P127页》5.6.7 --> ADC的时钟源取自于APB2时钟+分频
     【2】、根据《数据手册P82页》4.13 -->  ADC characteristics要求，最高的频率不超过14MHz！！！
     【3】、根据《数据手册P44页》2.6.4 --> 引脚分布图看来，本芯片只有10个ADC通道 

使用例子：
【1】、初始化ADC
     DRI_ADC_ChannelEnum AdcCh[] = {ADC_CH3_PA3,ADC_CH6_PA6,ADC_CH8_PB0,ADC_CH5_PA5};
     DRI_ADCCnfType adcconfig;
     adcconfig.AdcChNum = sizeof(AdcCh)/sizeof(AdcCh[0]);
     adcconfig.AdcReferenceValue = 3300;
     adcconfig.AdcChp = AdcCh;
     DRI_ADC_Config(&adcconfig);

【2】、读取ADC值
     s32 adcvalue = DRI_ADC0_Read(ADC_CH8_PB0);

*/
#include "DRI_ADC.h"
extern u32 DRI_ComDriver_OutDefaultDMAPriority(void);
extern s8 DRI_ComDriver_DMAChannelRegedit(dma_channel_enum channel);//注册DMA通道
extern s8 DRI_ComDriver_DMAChannelUnRegedit(dma_channel_enum channel);
/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下模块的对接函数区-----------------------------------------                                  *
*                                                                                                                                           *
********************************************************************************************************************************************/
#define ADC_ComDriver_OutDefaultDMAPriority()     DRI_ComDriver_OutDefaultDMAPriority()   //输出默认的DMA优先级
#define ADC_ComDriver_DMAChannelRegedit(CN)       DRI_ComDriver_DMAChannelRegedit(CN)     //注册DMA通道
#define ADC_ComDriver_DMAChannelUnRegedit(CN)     DRI_ComDriver_DMAChannelUnRegedit(CN)   //注销DMA通道
//
#define ADC_ComDriver_Delay1ms()                  DRI_ComDriver_Delay1ms()              //延时1ms
/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的变量申明和宏定义区--------------------------------                      *
*                                                                                                                                           *
********************************************************************************************************************************************/
//根据数据手册P82页 4.13.  ADC characteristics要求，最高的频率不超过14MHz！！！
#define ADC_MAX_CLK           14000000//芯片决定最大的时钟频率
#define ADC_MAX_ChannelNum    10   //芯片决定最大的通道数

typedef enum
{
     Adc_Status_NoInit = 0,    //未初始化
     Adc_Status_Free,          //空闲
     Adc_Status_Busy           //忙(转换中、或连续转换中)
}AdcStatusEnum;

typedef enum
{//值不可修改！！！
     ADCMode_DMAContinue = 0, //DMA连续模式
     ADCMode_DMASingle   = 1, //DMA单次模式
     ADCMode_Polling     = 2, //轮询模式---通道阻塞采样转换
}DRI_ADC_SampleModeEnum;//ADC采样转换模式枚举
/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的内部函数申明区------------------------------------                          *
*                                                                                                                                           *
********************************************************************************************************************************************/
static s8 AdcChannelIndex[ADC_MAX_ChannelNum];
static u16 AdcValue[ADC_MAX_ChannelNum];
static u16 AdcReferenceValue;

static AdcStatusEnum Adc0Status = Adc_Status_NoInit;//0:未初始化 1:空闲 2:转换中 3:转换完成
static DRI_ADC_SampleModeEnum Adc0WorkMode;//工作模式

static s8 ADC_CheckChannelRegedit(DRI_ADC_ChannelEnum cn,u8 index);
static void ADC_ConfigFailProcess(DRI_ADCCnfType *cnf);
static u8 ADC_DMAChannelRegeditOKFlag = 0;//DMA通道注册成功标志位
/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的系统函数代码区------------------------------------                          *
*                                                                                                                                           *
********************************************************************************************************************************************/
/// @brief ADC初始化
/// @param cnf 初始化结构体
/// @return 小于0表示失败，等于0表示成功
s8 DRI_ADC_Config(DRI_ADCCnfType *cnf)
{ 
     u8 u8temp;
     u32 AdcNum,u32temp;
     rcu_periph_enum rcu_adc;//
     //
     dma_parameter_struct dma_data_parameter;
     dma_channel_enum dma_channel;
     u32 dma_periph;

     //本芯片有ADC0 和 ADC1 共有2个ADC外设
     //有PA0(IN0)~PA7(IN7)，PB0(IN8)~PB1(IN9)，共10个ADC通道

     Adc0Status = Adc_Status_NoInit;
     ADC_DMAChannelRegeditOKFlag = 0;
     AdcNum = ADC0;
     rcu_adc = RCU_ADC0;

 //--------------------------------------------------------------------------------------------------    

     if((cnf->AdcChNum == 0) || (cnf->AdcReferenceValue == 0) || (cnf->AdcChNum > ADC_MAX_ChannelNum))
     {
          ADC_ConfigFailProcess(cnf);
          return -1;
     }

     Adc0WorkMode = ADCMode_DMAContinue;//cnf->AdcWorkMode;//工作模式
     AdcReferenceValue = cnf->AdcReferenceValue;//参考电压

     for(u8temp = 0;u8temp < ADC_MAX_ChannelNum;u8temp++)
     {
          AdcChannelIndex[u8temp] = -1;//通道索引号空闲
          AdcValue[u8temp] = 0;//通道值
     }

     //----------------------RCU配置     
     //使能ADC时钟
     rcu_periph_clock_enable(rcu_adc);     
     //根据数据手册P82页 4.13.  ADC characteristics要求，最高的频率不超过14MHz！！！
     u32temp = rcu_clock_freq_get(CK_APB2);
     if(u32temp / 2 <= ADC_MAX_CLK)
     {
          rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV2);
     }
     else if(u32temp / 4 <= ADC_MAX_CLK)
     {
          rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV4);
     }
     else if(u32temp / 6 <= ADC_MAX_CLK)
     {
          rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV6);
     }
     else if(u32temp / 8 <= ADC_MAX_CLK)
     {
          rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV8);
     }
     else if(u32temp / 12 <= ADC_MAX_CLK)
     {
          rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV12);
     }
     else if(u32temp / 16 <= ADC_MAX_CLK)
     {
          rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV16);
     }
     else 
     {
          ADC_ConfigFailProcess(cnf);
          return -2;
     }

     //----------------------GPIO配置
     for(u8temp = 0;u8temp < cnf->AdcChNum;u8temp++)
     {
          //需要增加ADC通道号相同的判断
          if(ADC_CheckChannelRegedit(cnf->AdcChp[u8temp],u8temp))
          {//ADC通道注册失败
               ADC_ConfigFailProcess(cnf);
               return -3;
          }
          
          //
          if(cnf->AdcChp[u8temp] <= ADC_CH7_PA7)
          {
               rcu_periph_clock_enable(RCU_GPIOA);
               gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, BIT(cnf->AdcChp[u8temp]));
          }
          else
          {
               rcu_periph_clock_enable(RCU_GPIOB);
               gpio_init(GPIOB, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, BIT(cnf->AdcChp[u8temp] - ADC_CH8_PB0));
          }          
     }


     


     //------------------------ADC配置     
     /* reset ADC */
     adc_deinit(AdcNum);
     /* ADC mode config */
     adc_mode_config(ADC_MODE_FREE);//自由运行模式
     /* ADC data alignment config */
     adc_data_alignment_config(AdcNum, ADC_DATAALIGN_RIGHT);//数据采用右对齐  
     /* ADC scan mode disable */
     adc_special_function_config(AdcNum, ADC_SCAN_MODE, ENABLE);//使用扫描模式   
     //常规通道序列配置----主要用于启动一次转换，实现多个通道按序列顺序转换。
     //常规序列通道--配置通道数
     adc_channel_length_config(AdcNum, ADC_REGULAR_CHANNEL, (u32)(cnf->AdcChNum));
     //常规序列通道--通道配置
     for(u8temp = 0;u8temp < cnf->AdcChNum;u8temp++)
     {
          adc_regular_channel_config(AdcNum, u8temp, cnf->AdcChp[u8temp], ADC_SAMPLETIME_55POINT5);//采样周期为55.5个ADC时钟周期
          //adc_regular_channel_config(AdcNum, 0, ADC_CHANNEL_3, ADC_SAMPLETIME_55POINT5);//IN3通道采样周期为55.5个ADC时钟周期
     }
     //常规序列通道--外部触发源配置
     adc_external_trigger_source_config(AdcNum, ADC_REGULAR_CHANNEL, ADC0_1_2_EXTTRIG_REGULAR_NONE);//常规序列通道--外部触发源配置--软件触发
     adc_external_trigger_config(AdcNum, ADC_REGULAR_CHANNEL, ENABLE);//常规组通道--外部触发源使能
     
     



     if((Adc0WorkMode == ADCMode_DMAContinue) || (Adc0WorkMode == ADCMode_DMASingle))
     {
          if(Adc0WorkMode == ADCMode_DMAContinue)
          {
               /* ADC contineous function enable */
               adc_special_function_config(AdcNum, ADC_CONTINUOUS_MODE, ENABLE);//连续转换模式
          }

          

//----------------------DMA配置
          //本芯片只有DMA0，且ADC硬件固定绑定在DMA0_CH0！！！
          //使能DMA0时钟
          rcu_periph_clock_enable(RCU_DMA0);
          dma_periph = DMA0; 
          dma_channel = DMA_CH0;
          


          if(ADC_ComDriver_DMAChannelRegedit(dma_channel))
          {//DMA通道注册失败-----其他外设已绑定在此通道上了。
               ADC_ConfigFailProcess(cnf);
               return -4;
          }

          ADC_DMAChannelRegeditOKFlag = 1;//DMA通道注册成功标志

          //DMA反初始化
          dma_deinit(dma_periph, dma_channel);          
          /* initialize DMA single data mode */
          dma_data_parameter.periph_addr  = (u32)(&ADC_RDATA(AdcNum));
          dma_data_parameter.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
          dma_data_parameter.memory_addr  = (u32)(&AdcValue);
          dma_data_parameter.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
          dma_data_parameter.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;
          dma_data_parameter.memory_width = DMA_MEMORY_WIDTH_16BIT;
          dma_data_parameter.direction    = DMA_PERIPHERAL_TO_MEMORY;
          dma_data_parameter.number       = (u32)(cnf->AdcChNum);//
          dma_data_parameter.priority     = DMA_PRIORITY_LOW;//ADC_ComDriver_OutDefaultDMAPriority();
          dma_init(dma_periph, dma_channel, &dma_data_parameter);
          //启用DMA内存访问循环模式
          dma_circulation_enable(dma_periph, dma_channel);
          //使能DMA通道
          dma_channel_enable(dma_periph, dma_channel);
          


          //ADC外设中的ADC_DMA请求使能
          adc_dma_mode_enable(AdcNum);//ADC_DMA请求使能
     }
     



     //使能ADC
     adc_enable(AdcNum);//使能ADC
     ADC_ComDriver_Delay1ms();

     // ADC校准 和 复位校准
     //adc_calibration_enable(AdcNum);//本芯片没有此功能





     Adc0Status = Adc_Status_Free;
     //
     if(Adc0WorkMode == ADCMode_DMAContinue)
     {//DMA连续模式
          Adc0Status = Adc_Status_Busy;
          //软件触发常规序列转换开始
          adc_software_trigger_enable(AdcNum, ADC_REGULAR_CHANNEL);
     }          

     return 0;
}


// { 
//      u32 u32temp;
//      rcu_periph_enum rcu_adc = RCU_ADC0;
//      //本芯片有ADC0 和 ADC1 共有2个ADC外设
//      //有PA0(IN0)~PA7(IN7)，PB0(IN8)~PB1(IN9)，共10个ADC通道
//      AdcNum = ADC0;
//      //----------------------GPIO配置
//      /* config the GPIO as analog mode */
//      /* enable GPIOA clock */
//      rcu_periph_clock_enable(RCU_GPIOA);
//      //gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
//      gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_3|GPIO_PIN_5);
//      //----------------------RCU配置     
//      /* enable ADC clock */
//      rcu_periph_clock_enable(rcu_adc);     
//      //根据数据手册P82页 4.13.  ADC characteristics要求，最高的频率不超过14MHz！！！
//      u32temp = rcu_clock_freq_get(CK_APB2);
//      if(u32temp / 2 <= ADC_MAX_CLK)
//      {
//           rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV2);
//      }
//      else if(u32temp / 4 <= ADC_MAX_CLK)
//      {
//           rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV4);
//      }
//      else if(u32temp / 6 <= ADC_MAX_CLK)
//      {
//           rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV6);
//      }
//      else if(u32temp / 8 <= ADC_MAX_CLK)
//      {
//           rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV8);
//      }
//      else if(u32temp / 12 <= ADC_MAX_CLK)
//      {
//           rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV12);
//      }
//      else if(u32temp / 16 <= ADC_MAX_CLK)
//      {
//           rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV16);
//      }
//      else 
//      {
//           return -1;
//      }
//      //------------------------ADC配置
//      /* reset ADC */
//      adc_deinit(AdcNum);
//      /* ADC mode config */
//      adc_mode_config(ADC_MODE_FREE);//自由运行模式
//      /* ADC contineous function enable */
//      adc_special_function_config(AdcNum, ADC_CONTINUOUS_MODE, ENABLE);//连续转换模式
//      /* ADC scan mode disable */
//      adc_special_function_config(AdcNum, ADC_SCAN_MODE, ENABLE);//使用扫描模式
//      /* ADC data alignment config */
//      adc_data_alignment_config(AdcNum, ADC_DATAALIGN_RIGHT);//数据采用右对齐
//      //常规通道序列配置----主要用于启动一次转换，实现多个通道按序列顺序转换。
//      //常规序列通道--配置通道数
//      adc_channel_length_config(AdcNum, ADC_REGULAR_CHANNEL, 2);
//      //常规序列通道--通道配置
//      // adc_regular_channel_config(ADC0, 0, ADC_CHANNEL_0, ADC_SAMPLETIME_55POINT5);
//      // adc_regular_channel_config(ADC0, 1, ADC_CHANNEL_1, ADC_SAMPLETIME_55POINT5);
//      // adc_regular_channel_config(ADC0, 2, ADC_CHANNEL_2, ADC_SAMPLETIME_55POINT5);
//      // adc_regular_channel_config(ADC0, 3, ADC_CHANNEL_3, ADC_SAMPLETIME_55POINT5);
//      adc_regular_channel_config(AdcNum, 0, ADC_CHANNEL_3, ADC_SAMPLETIME_55POINT5);//IN3通道采样周期为55.5个ADC时钟周期
//      adc_regular_channel_config(AdcNum, 1, ADC_CHANNEL_5, ADC_SAMPLETIME_55POINT5);//IN5通道采样周期为55.5个ADC时钟周期
//      //常规序列通道--外部触发源配置
//      adc_external_trigger_source_config(AdcNum, ADC_REGULAR_CHANNEL, ADC0_1_2_EXTTRIG_REGULAR_NONE);//常规序列通道--外部触发源配置--软件触发
//      adc_external_trigger_config(AdcNum, ADC_REGULAR_CHANNEL, ENABLE);//常规组通道--外部触发源使能     
//      //使能ADC
//      adc_enable(AdcNum);//使能ADC
//      DRI_ComDriver_Delay1ms();
//      // ADC校准 和 复位校准
//      //adc_calibration_enable(AdcNum);//本芯片没有此功能
//      //----------------------DMA配置
//      /* ADC_DMA_channel configuration */
//      dma_parameter_struct dma_data_parameter;
//      /* enable DMA0 clock */
//      rcu_periph_clock_enable(RCU_DMA0);
//      /* ADC DMA_channel configuration */
//      dma_deinit(DMA0, DMA_CH0);     
//      /* initialize DMA single data mode */
//      dma_data_parameter.periph_addr  = (u32)(&ADC_RDATA(AdcNum));
//      dma_data_parameter.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
//      dma_data_parameter.memory_addr  = (u32)(&adc_value);
//      dma_data_parameter.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
//      dma_data_parameter.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;
//      dma_data_parameter.memory_width = DMA_MEMORY_WIDTH_16BIT;
//      dma_data_parameter.direction    = DMA_PERIPHERAL_TO_MEMORY;
//      dma_data_parameter.number       = 2U;//sizeof(adc_value)/sizeof(adc_value[0]);
//      dma_data_parameter.priority     = DRI_ComDriver_OutDefaultDMAPriority();
//      dma_init(DMA0, DMA_CH0, &dma_data_parameter);
//      dma_circulation_enable(DMA0, DMA_CH0);     
//      /* enable DMA channel */
//      dma_channel_enable(DMA0, DMA_CH0);
//      //ADC外设中的ADC_DMA请求使能
//      adc_dma_mode_enable(AdcNum);//ADC_DMA请求使能
//      //软件触发常规序列转换开始
//      adc_software_trigger_enable(AdcNum, ADC_REGULAR_CHANNEL);
//      return 0;
// }


/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的用户函数代码区------------------------------------                          *
*                                                                                                                                           *
********************************************************************************************************************************************/
/// @brief 读取指定通道的电压值，单位mV
/// @param cn 指定通道
/// @return 小于0:读取失败 大等于0:读取的电压值，单位mV
s32 DRI_ADC0_Read(DRI_ADC_ChannelEnum cn)
{
     s8 index;
     s32 s32result = -1;

     if(Adc0Status == Adc_Status_NoInit)
     {//未初始化
          return -2;
     }

     if(cn > ADC_CH9_PB1)
     {//非法通道
          return -3;
     }

     index = AdcChannelIndex[cn];
     if(index < 0)
     {//通道合法，但不存在
          return -4;
     }

     if(Adc0WorkMode == ADCMode_DMAContinue)
     {
          if(Adc0Status == Adc_Status_Free)
          {
               Adc0Status = Adc_Status_Busy;
               adc_software_trigger_enable(ADC0, ADC_REGULAR_CHANNEL);
               s32result = AdcValue[index];
          }
          else if(Adc0Status == Adc_Status_Busy)
          {//连续DMA模式，返回当前值
               s32result = AdcValue[index];
          }
     }
     else if(Adc0WorkMode == ADCMode_DMASingle)
     {

     }


     //ADC值 转为 电压mV
     if(s32result >= 0)
     {
          s32result = (AdcReferenceValue * s32result) / 4096;
     }

     return s32result;
}
/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的内部函数代码区------------------------------------                          *
*                                                                                                                                           *
********************************************************************************************************************************************/
//判断并注册ADC通道
//cn:ADC通道 index:ADC通道索引
//返回值：小于0注册失败，等于0注册成功
static s8 ADC_CheckChannelRegedit(DRI_ADC_ChannelEnum cn,u8 index)
{//
     if(cn > ADC_CH9_PB1)
     {//非法ADC通道
          return -1;
     }
     
     //判断是否已经注册
     if(AdcChannelIndex[cn] >= 0)
     {//已被注册
          return -2;
     }

     AdcChannelIndex[cn] = index;
     return 0;//注册成功
}

static void ADC_ChannelUnRegedit(DRI_ADC_ChannelEnum cn)
{
     AdcChannelIndex[cn] = -1;
}


static void ADC_ConfigFailProcess(DRI_ADCCnfType *cnf)
{
     u8 u8i;
     for(u8i = 0;u8i < cnf->AdcChNum;u8i++)
     {
          ADC_ChannelUnRegedit(cnf->AdcChp[u8i]);
     }
     if(ADC_DMAChannelRegeditOKFlag)
     {
          ADC_ComDriver_DMAChannelUnRegedit(DMA_CH0);
     }     
}






