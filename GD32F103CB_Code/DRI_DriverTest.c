/********************************************************************************************************************************************
*                                                                                                                                           *
*              ---------------------------------以下是模块的修改记录区-----------------------------------------                             *
*                                                                                                                                           *
********************************************************************************************************************************************/
/**********************************************
 * 内  容：
 * 日  期：2025-09-17
 * 作  者：
 * 版本号：V1.0（初版）
 ***********************************************
 * 修改内容：
 * 修改日期：
 * 修改作者：
 * 版 本 号：
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


#define DRI_Test_None    0
#define DRI_Test_USART0  1
#define DRI_Test_Flash   2


#define DRI_DriverTest_Obj    DRI_Test_None


#include "DRI_DriverTest.h"
#if DRI_DriverTest_Obj != DRI_Test_None 
#include <stdio.h>
#include "DRI_USARTn.h"
#include "DRI_Flash.h"
#endif
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

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的内部函数申明区------------------------------------                          *
*                                                                                                                                           *
********************************************************************************************************************************************/
#if DRI_DriverTest_Obj != DRI_Test_None  
static s8 DriverTest_USART0_Init(void);
static void DriverTest_Flash_Init(void);
#endif
/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的系统函数代码区------------------------------------                          *
*                                                                                                                                           *
********************************************************************************************************************************************/

/// @brief 驱动测试初始化函数
/// @param  
void DRI_DriverTest_Init(void)
{
#if DRI_DriverTest_Obj != DRI_Test_None
     DriverTest_USART0_Init();//此处需要使用该串口来打印测试信息
     switch(DRI_DriverTest_Obj)
     {
          case DRI_Test_USART0: 
               break;
          case DRI_Test_Flash:
               DriverTest_Flash_Init();
               break;
          default:
               break;
     }
#endif
}

/// @brief 驱动测试实时任务
/// @param  
void DRI_DriverTest_RealTask(void)
{
#if DRI_DriverTest_Obj != DRI_Test_None     
     switch(DRI_DriverTest_Obj)
     {
          case DRI_Test_USART0:
               break;
          case DRI_Test_Flash:
               break;
          default:
               break;
     }
#endif     
}
/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的用户函数代码区------------------------------------                          *
*                                                                                                                                           *
********************************************************************************************************************************************/


/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的内部函数代码区------------------------------------                          *
*                                                                                                                                           *
********************************************************************************************************************************************/
#if DRI_DriverTest_Obj != DRI_Test_None
int fputc(int ch, FILE *f)
{
    DRI_USART0_SendByte((u8)ch);
    return ch;
}

static void DriverTest_USART0_ReceData(u8 data)
{
     DRI_USART0_SendByte(data);
}

static s8 DriverTest_USART0_Init(void)
{//配置波特率115200，8位数据位，1位停止位，无校验位
     DRI_USARTCnfType uartcnf;
     uartcnf.bps = 115200;
     uartcnf.DataBit = DataBits8;
     uartcnf.StopBit = OneStopBit;
     uartcnf.Parity = ParityNone;
     uartcnf.rhwfc.FEnable = 0;//禁用硬流控
     
     uartcnf.IntPri = 10;
     uartcnf.RecData_IntEnable = 1;
     uartcnf.Recfp = DriverTest_USART0_ReceData;
     uartcnf.SendOK_IntEnable = 0;
     uartcnf.SendINTDefaultState = 0;
     uartcnf.SendOKfp = NULL;
     return DRI_USART0_Config(&uartcnf);
}

//------------------------------------------------------
static void DriverTest_Flash_Init(void)
{
     u8 ssn = 127;//开始扇区号
     u8 si,testbuffer[100];
     u16 u16n,u16i;
     s32 s32n;
     u32 u32eradd,offset;

     printf("\r\n\r\n开始Flash测试\r\n");
     
     u32eradd = 0x08000000 + ssn * 1024;
     if(DRI_Flash_EraseSector(u32eradd) != 1024)
     {
          printf("扇区擦除失败*********************************\r\n");
          return;
     } 
     for(u16i = 0; u16i < sizeof(testbuffer); u16i++)
     {
          testbuffer[u16i] = 0;
     }
     s32n = DRI_Flash_ReadData(u32eradd, testbuffer, sizeof(testbuffer));
     if(s32n != sizeof(testbuffer))
     {
          printf("地址【0x%08x】读取擦除数据失败*********************************\r\n", u32eradd);
          return;
     }
     for(u16i = 0; u16i < sizeof(testbuffer); u16i++)
     {
          if(testbuffer[u16i] != 0xff)
          {
               printf("地址【0x%08x】校验擦除数据失败*********************************\r\n", u32eradd);
               return;
          }
     }
//----------------------------------------------------------
     u16n = 4;


     for(u16i = 0; u16i < sizeof(testbuffer); u16i++)
     {
          testbuffer[u16i] = u16i % 256;
     }
     DRI_Flash_DirectWriteData(u32eradd, testbuffer, u16n);
 //--------------------------------------------------------    
     for(u16i = 0; u16i < sizeof(testbuffer); u16i++)
     {
          testbuffer[u16i] = 0;
     }
     s32n = DRI_Flash_ReadData(u32eradd, testbuffer, u16n);
     if(s32n != u16n)
     {
          printf("地址【0x%08x】读取写入数据失败*********************************\r\n", u32eradd);
          return;
     }
     for(u16i = 0; u16i < u16n; u16i++)
     {
          if(testbuffer[u16i] != u16i % 256)
          {
               printf("地址【0x%08x】校验写入数据失败,基准值:0x%02X,读到值:0x%02X*********************************\r\n", u32eradd + u16i,u16i % 256,testbuffer[u16i]);
               return;
          }
     }



     







     // printf("\r\n\r\n开始Flash测试\r\n");
     
     // u32eradd = 0x08000000 + ssn * 1024;
     // if(DRI_Flash_EraseSector(u32eradd) != 1024)
     // {
     //      printf("扇区擦除失败*********************************\r\n");
     //      return;
     // } 
     // for(u16i = 0; u16i < sizeof(testbuffer); u16i++)
     // {
     //      testbuffer[u16i] = 0;
     // }
     // s32n = DRI_Flash_ReadData(u32eradd, testbuffer, sizeof(testbuffer));
     // if(s32n != sizeof(testbuffer))
     // {
     //      printf("地址【0x%08x】读取擦除数据失败*********************************\r\n", u32eradd);
     //      return;
     // }
     // for(u16i = 0; u16i < sizeof(testbuffer); u16i++)
     // {
     //      if(testbuffer[u16i] != 0xff)
     //      {
     //           printf("地址【0x%08x】校验擦除数据失败*********************************\r\n", u32eradd);
     //           return;
     //      }
     // }

     // offset = 0;//1020;
     // u16n = 2;
     // printf("开始-------测试地址【0x%08x】\r\n",u32eradd + offset);

     // printf("测试地址【0x%08x】\r\n",u32eradd + offset);
     // for(u16i = 0; u16i < sizeof(testbuffer); u16i++)
     // {
     //      testbuffer[u16i] = u16i % 256;
     // }
     // if(DRI_Flash_DirectWriteData(u32eradd + offset, testbuffer, u16n) != u16n)
     // {
     //      printf("地址【0x%08x】写入失败*********************************\r\n", u32eradd + offset);
     //      return;
     // }

     // for(u16i = 0; u16i < sizeof(testbuffer); u16i++)
     // {
     //      testbuffer[u16i] = 0xff;
     // }
     // s32n = DRI_Flash_ReadData(u32eradd, testbuffer, sizeof(testbuffer));
     // if(s32n != sizeof(testbuffer))
     // {
     //      printf("地址【0x%08x】读取失败*********************************\r\n", u32eradd);
     //      return;
     // }

     // for(u16i = 0; u16i < sizeof(testbuffer); u16i++)
     // {
     //      if((u16i >= offset) && (u16i < (offset + u16n)))
     //      {//如果大于偏移量，则开始
     //           if(testbuffer[u16i] != (u16i - offset) % 256)
     //           {
     //                printf("地址【0x%08x】基准值:%d,实际值:%d------错误1*********************************\r\n", u32eradd + u16i, (u16i - offset) % 256,testbuffer[u16i]);
     //                return;
     //           }
     //      }
     //      else
     //      {
     //           if(testbuffer[u16i] != 0xff)
     //           {
     //                printf("地址【0x%08x】基准值:%d,实际值:%d------错误0*********************************\r\n", u32eradd + u16i,0xff,testbuffer[u16i]);
     //                return;
     //           }
     //      }
     // }


     



     //-----------------------------------------------------
     printf("Flash测试完成VVVVVVVVVVVVVVVVVVVVVVVVVVV\r\n");
}

#endif
