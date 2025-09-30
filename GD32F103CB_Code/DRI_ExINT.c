/********************************************************************************************************************************************
*                                                                                                                                           *
*              ---------------------------------以下是模块的修改记录区-----------------------------------------                             *
*                                                                                                                                           *
********************************************************************************************************************************************/
/**********************************************
 * 内容：
 * 日期：2025-09-02
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
     

*/
#include "DRI_ExINT.h"
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
// #define GPIO_PERIPH(PortN)    (GPIOA + (PortN * 0x00000400U))
// #define GPIO_PIN(PinN)        BIT(PinN)

//
static void (*ExINT_CallBackF00)(void) = NULLFP;
static void (*ExINT_CallBackF01)(void) = NULLFP;
static void (*ExINT_CallBackF02)(void) = NULLFP;
static void (*ExINT_CallBackF03)(void) = NULLFP;
static void (*ExINT_CallBackF04)(void) = NULLFP;
static void (*ExINT_CallBackF05)(void) = NULLFP;
static void (*ExINT_CallBackF06)(void) = NULLFP;
static void (*ExINT_CallBackF07)(void) = NULLFP;
static void (*ExINT_CallBackF08)(void) = NULLFP;
static void (*ExINT_CallBackF09)(void) = NULLFP;
static void (*ExINT_CallBackF10)(void) = NULLFP;
static void (*ExINT_CallBackF11)(void) = NULLFP;
static void (*ExINT_CallBackF12)(void) = NULLFP;
static void (*ExINT_CallBackF13)(void) = NULLFP;
static void (*ExINT_CallBackF14)(void) = NULLFP;
static void (*ExINT_CallBackF15)(void) = NULLFP;
//
// typedef void (*ExINT_CBFP)(void);
// static ExINT_CBFP PA_ExCBFP[] = {ExINT_CallBackFPA001,ExINT_CallBackFPA011};
/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的内部函数申明区------------------------------------                          *
*                                                                                                                                           *
********************************************************************************************************************************************/
// static void ExINT_CBF00(void);
// static void ExINT_CBF01(void);
// static void ExINT_CBF02(void);
// static void ExINT_CBF03(void);
// static void ExINT_CBF04(void);
// static void ExINT_CBF05(void);
// static void ExINT_CBF06(void);
// static void ExINT_CBF07(void);
// static void ExINT_CBF08(void);
// static void ExINT_CBF09(void);
// static void ExINT_CBF10(void);
// static void ExINT_CBF11(void);
// static void ExINT_CBF12(void);
// static void ExINT_CBF13(void);
// static void ExINT_CBF14(void);
// static void ExINT_CBF15(void);

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的系统函数代码区------------------------------------                          *
*                                                                                                                                           *
********************************************************************************************************************************************/


/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的用户函数代码区------------------------------------                          *
*                                                                                                                                           *
********************************************************************************************************************************************/
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
s8 DRI_ExINT_Config(ExIntCnfType *eict)
{
     u8 exit_port,exit_pin;
     exti_line_enum linex;
     exti_trig_type_enum trig_type;//中断触发类型
     IRQn_Type nvic_irq;//中断向量号    
     u32 gpio_periph,pin;
     
     rcu_periph_clock_enable(RCU_AF);//使能复用功能时钟

     switch(eict->portn)
     {
          case GPORTA:
               rcu_periph_clock_enable(RCU_GPIOA);
               exit_port = GPIO_PORT_SOURCE_GPIOA;
               gpio_periph = GPIOA;
               break;
          case GPORTB:
               rcu_periph_clock_enable(RCU_GPIOB);
               exit_port = GPIO_PORT_SOURCE_GPIOB;
               gpio_periph = GPIOB;
               break;
          case GPORTC:
               rcu_periph_clock_enable(RCU_GPIOC);
               exit_port = GPIO_PORT_SOURCE_GPIOC;
               gpio_periph = GPIOC;
               break;
          default:
               return -1;
     }

     switch(eict->pinn)
     {
          case GP00:
               pin = GPIO_PIN_0;
               exit_pin = GPIO_PIN_SOURCE_0;
               linex = EXTI_0;
               nvic_irq = EXTI0_IRQn;
               ExINT_CallBackF00 = eict->cbfp;
               break;
          case GP01:
               pin = GPIO_PIN_1;
               exit_pin = GPIO_PIN_SOURCE_1;
               linex = EXTI_1;
               nvic_irq = EXTI1_IRQn;
               ExINT_CallBackF01 = eict->cbfp;
               break;
          case GP02:
               pin = GPIO_PIN_2;
               exit_pin = GPIO_PIN_SOURCE_2;
               linex = EXTI_2;
               nvic_irq = EXTI2_IRQn;
               ExINT_CallBackF02 = eict->cbfp;
               break;
          case GP03:
               pin = GPIO_PIN_3;
               exit_pin = GPIO_PIN_SOURCE_3;
               linex = EXTI_3;
               nvic_irq = EXTI3_IRQn;
               ExINT_CallBackF03 = eict->cbfp;
               break;
          case GP04:
               pin = GPIO_PIN_4;
               exit_pin = GPIO_PIN_SOURCE_4;
               linex = EXTI_4;
               nvic_irq = EXTI4_IRQn;
               ExINT_CallBackF04 = eict->cbfp;
               break;
          case GP05:
               pin = GPIO_PIN_5;
               exit_pin = GPIO_PIN_SOURCE_5;
               linex = EXTI_5;
               nvic_irq = EXTI5_9_IRQn;
               ExINT_CallBackF05 = eict->cbfp;
               break;
          case GP06:
               pin = GPIO_PIN_6;
               exit_pin = GPIO_PIN_SOURCE_6;
               linex = EXTI_6;
               nvic_irq = EXTI5_9_IRQn;
               ExINT_CallBackF06 = eict->cbfp;
               break;
          case GP07:
               pin = GPIO_PIN_7;
               exit_pin = GPIO_PIN_SOURCE_7;
               linex = EXTI_7;
               nvic_irq = EXTI5_9_IRQn;
               ExINT_CallBackF07 = eict->cbfp;
               break;
          case GP08:
               pin = GPIO_PIN_8;
               exit_pin = GPIO_PIN_SOURCE_8;
               linex = EXTI_8;
               nvic_irq = EXTI5_9_IRQn;
               ExINT_CallBackF08 = eict->cbfp;
               break;
          case GP09:
               pin = GPIO_PIN_9;
               exit_pin = GPIO_PIN_SOURCE_9;
               linex = EXTI_9;
               nvic_irq = EXTI5_9_IRQn;
               ExINT_CallBackF09 = eict->cbfp;
               break;
          case GP10:
               pin = GPIO_PIN_10;
               exit_pin = GPIO_PIN_SOURCE_10;
               linex = EXTI_10;
               nvic_irq = EXTI10_15_IRQn;
               ExINT_CallBackF10 = eict->cbfp;
               break;
          case GP11:
               pin = GPIO_PIN_11;
               exit_pin = GPIO_PIN_SOURCE_11;
               linex = EXTI_11;
               nvic_irq = EXTI10_15_IRQn;
               ExINT_CallBackF11 = eict->cbfp;
               break;
          case GP12:
               pin = GPIO_PIN_12;
               exit_pin = GPIO_PIN_SOURCE_12;
               linex = EXTI_12;
               nvic_irq = EXTI10_15_IRQn;
               ExINT_CallBackF12 = eict->cbfp;
               break;
          case GP13:
               pin = GPIO_PIN_13;
               exit_pin = GPIO_PIN_SOURCE_13;
               linex = EXTI_13;
               nvic_irq = EXTI10_15_IRQn;
               ExINT_CallBackF13 = eict->cbfp;
               break;
          case GP14:
               pin = GPIO_PIN_14;
               exit_pin = GPIO_PIN_SOURCE_14;
               linex = EXTI_14;
               nvic_irq = EXTI10_15_IRQn;
               ExINT_CallBackF14 = eict->cbfp;
               break;
          case GP15:
               pin = GPIO_PIN_15;
               exit_pin = GPIO_PIN_SOURCE_15;
               linex = EXTI_15;
               nvic_irq = EXTI10_15_IRQn;
               ExINT_CallBackF15 = eict->cbfp;
               break;
          default:
               return -2;
     }
     //
     switch(eict->tm)
     {
          case ITFallEdge:
               trig_type = EXTI_TRIG_FALLING;
               break;
          case ITRiseEdge:
               trig_type = EXTI_TRIG_RISING;
               break;
          case ITBothEdg:
               trig_type = EXTI_TRIG_BOTH;
               break;
          default:  
               return -3;
     }
     
     //
     gpio_init(gpio_periph, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ,pin);
     //gpio_init(GPIO_PERIPH(eict->portn), GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ,GPIO_PIN(eict->pinn));
     // gpio_init(GPIO_PERIPH(eict->portn), GPIO_MODE_IPU, GPIO_OSPEED_50MHZ,GPIO_PIN(eict->pinn));

     ///* connect key EXTI line to key GPIO pin */
     gpio_exti_source_select(exit_port, exit_pin);
     //
     exti_init(linex, EXTI_INTERRUPT, trig_type);
     exti_interrupt_flag_clear(linex);

     // 
     NVIC_ClearPendingIRQ(nvic_irq);//清除挂起标志     
     NVIC_SetPriority(nvic_irq, eict->pri);//设置中断优先级     
     NVIC_EnableIRQ(nvic_irq);//使能NVIC中断
     //nvic_irq_enable(nvic_irq, 2U, 0U); 

     return 0;
}



void EXTI0_IRQHandler(void)
{
     if(RESET != exti_interrupt_flag_get(EXTI_0)) 
     {
          ExINT_CallBackF00();
          exti_interrupt_flag_clear(EXTI_0);
     }
}

void EXTI1_IRQHandler(void)
{
     if(RESET != exti_interrupt_flag_get(EXTI_1)) 
     {
          ExINT_CallBackF01();
          exti_interrupt_flag_clear(EXTI_1);
     }
}

void EXTI2_IRQHandler(void)
{
     if(RESET != exti_interrupt_flag_get(EXTI_2)) 
     {
          ExINT_CallBackF02();
          exti_interrupt_flag_clear(EXTI_2);
     }
}

void EXTI3_IRQHandler(void)
{
     if(RESET != exti_interrupt_flag_get(EXTI_3)) 
     {
          ExINT_CallBackF03();
          exti_interrupt_flag_clear(EXTI_3);
     }
}

void EXTI4_IRQHandler(void)
{
     if(RESET != exti_interrupt_flag_get(EXTI_4)) 
     {
          ExINT_CallBackF04();
          exti_interrupt_flag_clear(EXTI_4);
     }
}

void EXTI5_9_IRQHandler(void)
{
     if(RESET != exti_interrupt_flag_get(EXTI_5)) 
     {
          ExINT_CallBackF05();
          exti_interrupt_flag_clear(EXTI_5);
     }
     if(RESET != exti_interrupt_flag_get(EXTI_6)) 
     {
          ExINT_CallBackF06();
          exti_interrupt_flag_clear(EXTI_6);
     }
     if(RESET != exti_interrupt_flag_get(EXTI_7)) 
     {
          ExINT_CallBackF07();
          exti_interrupt_flag_clear(EXTI_7);
     }
     if(RESET != exti_interrupt_flag_get(EXTI_8)) 
     {
          ExINT_CallBackF08();
          exti_interrupt_flag_clear(EXTI_8);
     }
     if(RESET != exti_interrupt_flag_get(EXTI_9)) 
     {
          ExINT_CallBackF09();
          exti_interrupt_flag_clear(EXTI_9);
     }
}

void EXTI10_15_IRQHandler(void)
{
     if(RESET != exti_interrupt_flag_get(EXTI_10)) 
     {
          ExINT_CallBackF10();
          exti_interrupt_flag_clear(EXTI_10);
     }
     if(RESET != exti_interrupt_flag_get(EXTI_11)) 
     {
          ExINT_CallBackF11();
          exti_interrupt_flag_clear(EXTI_11);
     }
     if(RESET != exti_interrupt_flag_get(EXTI_12)) 
     {
          ExINT_CallBackF12();
          exti_interrupt_flag_clear(EXTI_12);
     }
     if(RESET != exti_interrupt_flag_get(EXTI_13)) 
     {
          ExINT_CallBackF13();
          exti_interrupt_flag_clear(EXTI_13);
     }
     if(RESET != exti_interrupt_flag_get(EXTI_14)) 
     {
          ExINT_CallBackF14();
          exti_interrupt_flag_clear(EXTI_14);
     }
     if(RESET != exti_interrupt_flag_get(EXTI_15)) 
     {
          ExINT_CallBackF15();
          exti_interrupt_flag_clear(EXTI_15);
     }
}

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的内部函数代码区------------------------------------                          *
*                                                                                                                                           *
********************************************************************************************************************************************/


