/********************************************************************************************************************************************
*                                                                                                                                           *
*              ---------------------------------以下是模块的修改记录区-----------------------------------------                             *
*                                                                                                                                           *
********************************************************************************************************************************************/
/**********************************************
 * 内容：
 * 日期：2025-09-01
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
#include "DRI_Flash.h"
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
#define Flash_SpaceStartAddr  0x08000000//flash空间起始地址
#define Flash_SpaceEndAddr    0x0801FFFF//flash空间结束地址
//
#define Flash_MaxSpaceSize    (Flash_SpaceEndAddr - Flash_SpaceStartAddr + 1)//flash大小，单位字节
#define Flash_MinSectorSize   1024//最小扇区大小，单位字节
/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的内部函数申明区------------------------------------                          *
*                                                                                                                                           *
********************************************************************************************************************************************/
static u16 Flash_WriteData(u32 wadd,u8 *wd,u16 wl);
static s32 Flash_SectorErase(u32 secaddr);
static void Flash_SectorProcess(u32 addr,u16 ldata,u16 mfl,u16 rdata);
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
* 函 数 名: DRI_Flash_ReadData
* 功能描述：Flash 阻塞型 读数据函数
*          支持任意地址任意大小的读取
* 入口参数：
            u32 radd:读地址
            u8 *rd:数据存放首地址
            u16 rl:读字节数
* 出口参数：
       其他值:实际读取字节数
        小于0:操作失败
* 注意事项:  
            无
* 例     如:
* 修改记录:
*           2025-09-01 BY:YJX
***************************************************************************/
s32 DRI_Flash_ReadData(u32 radd,u8 *rd,u16 rl)
{
     u8 *rp = (u8 *)radd;
     u16 u16temp;

     if(radd < Flash_SpaceStartAddr || radd > Flash_SpaceEndAddr || (radd + rl) > (Flash_SpaceEndAddr + 1))
     {//越界了
          return -1;
     }

     if(rl)
     {
          for(u16temp = 0;u16temp < rl;u16temp++)
          {
               rd[u16temp] = rp[u16temp];
          }
     }
     return rl;
}

/***************************************************************************
* 函 数 名: DRI_Flash_DirectWriteData
* 功能描述：Flash 阻塞型直写数据函数(不擦除直接写入)
*          支持任意地址任意大小的写入
* 入口参数：
            u32 wadd:写地址
            u8 *wd:数据源首地址
            u16 wl:写字节数
* 出口参数：
       其他值:实际写字节数
        小于0:操作失败
* 注意事项:  
            不擦除直接写入!!!
* 例   如:
* 修改记录:
*           2025-09-01 BY:YJX
***************************************************************************/
s32 DRI_Flash_DirectWriteData(u32 wadd,u8 *wd,u16 wl)
{//
     u8 u8temp,*rp,u8i,u8data[4];
     u16 u16temp,u16wl;

     if(wadd < Flash_SpaceStartAddr || wadd > Flash_SpaceEndAddr || (wadd + wl) > (Flash_SpaceEndAddr + 1))
     {//越界了
          return -1;
     }

     u16wl = wl;//暂存wl

     if(wl)
     {
          //处理首部非4字节对齐
          u8temp = (u8)(wadd % 4);
          if(u8temp)
          {//首部非4字节对齐
               rp = (u8 *)(wadd - u8temp);//往前u8temp个字节，读u8temp个数据
               for(u8i = 0;u8i < u8temp;u8i++)
               {
                    u8data[u8i] = rp[u8i];
               }
               for(u8i = u8temp;u8i < 4;u8i++)
               {
                    u8data[u8i] = wd[u8i - u8temp];
               }
               //对一个完整且对齐的4字节进行写操作
               Flash_WriteData((u32)rp,u8data,4);
               //更新写入地址、源数据指针、写入字节数
               u8temp = 4 - u8temp;
               wadd += u8temp;
               wd += u8temp;
               wl -= u8temp;
          }

          //写入中间部分
          u16temp = Flash_WriteData(wadd,wd,wl);
          wadd += u16temp;
          wd += u16temp;
          wl -= u16temp;

          //处理尾部非4字节对齐
          u8temp = (u8)(wl % 4);
          if(u8temp)
          {
               for(u8i = 0;u8i < u8temp;u8i++)
               {
                    u8data[u8i] = wd[u8i];
               }
               rp = (u8 *)(wadd + u8temp);
               for(u8i = u8temp;u8i < 4;u8i++)
               {
                    u8data[u8i] = rp[u8i - u8temp];
               }
               //对一个完整且对齐的4字节进行写操作
               Flash_WriteData((u32)wadd,u8data,4);
          }
     }

     return u16wl;
}

/// @brief 判断地址是否扇区头对齐 或 尾对齐
/// @param addr 待判断的地址
/// @return 小于0:表示不对齐 0:表示头对齐 大于0:表示尾对齐
s8 DRI_Flash_IsSectorAligned(u32 addr)
{
     if(addr < Flash_SpaceStartAddr || addr > Flash_SpaceEndAddr)
     {
          return -1;
     }

     if(addr % Flash_MinSectorSize == 0)
     {//头对齐
          return 0;
     }

     if((addr + 1) % Flash_MinSectorSize == 0)
     {//尾对齐
          return 1;
     }

     return -2;
}

/// @brief 擦除eaddr所在的整个扇区的函数
/// @param eaddr ：要擦除的地址
/// @return ：小于0:擦除失败 其他值：实际擦除字节数 
s32 DRI_Flash_EraseSector(u32 eaddr)
{
     return Flash_SectorErase(eaddr);
}


/***************************************************************************
* 函 数 名: DRI_Flash_Erase
* 功能描述：Flash 阻塞型 空间擦除函数
* 入口参数：
            u32 eaddr：擦除起始地址
            u32 esize：擦除字节数
* 出口参数：
       其他值:实际擦除字节数
        小于0:擦除失败
* 注意事项:
        支持任意地址任意大小的擦除
        如果地址和大小都是1024字节对齐，则栈空间不会被占用1024字节，否则栈空间会占用1024字节
* 例    如:
* 修改记录:
*           2025-09-01 BY:YJX
***************************************************************************/
s32 DRI_Flash_Erase(u32 eaddr,u32 esize)
{
     u16 u16temp,u16l,u16m,u16r,u16len;
     u32 secaddr;

     if(eaddr + esize > Flash_MaxSpaceSize)
     {//越界了
          //printf("越界了，eaddr = 0x%08x,esize = %d",eaddr,esize);
          return -1;
     }

     u16len = esize;

     if(esize)
     {
     //-----处理前部
          u16temp = (eaddr % Flash_MinSectorSize);
          if(u16temp)
          {//地址不对齐---判断是否只删除中间扇区
               secaddr = eaddr - u16temp;//使得地址为本扇区对齐
               u16l = u16temp;//扇区前面原数据字节数
               if((u16temp + esize) < Flash_MinSectorSize)
               {//仅扇区中间
                    u16m = esize;//扇区中间删除字节数
                    u16r = (Flash_MinSectorSize - u16temp - esize);//扇区后面原数据字节数
               }
               else
               {//跨扇区
                    u16m = (Flash_MinSectorSize - u16temp);//扇区中间删除字节数
                    u16r = 0;//扇区后面原数据字节数
               }
               //扇区擦除
               Flash_SectorProcess(secaddr,u16l,u16m,u16r);
               //更新参数
               eaddr += u16m;
               esize -= u16m;
          }          
     
     //-----处理中部
          while(esize >= Flash_MinSectorSize)
          {
               Flash_SectorErase(eaddr);
               eaddr += Flash_MinSectorSize;
               esize -= Flash_MinSectorSize;
          }

     //-----处理后部
          if(esize)
          {
               secaddr = eaddr;
               u16l = 0;//扇区前面原数据字节数
               u16m = esize;//扇区中间删除字节数
               u16r = (Flash_MinSectorSize - esize);//扇区后面原数据字节数
               //扇区擦除
               Flash_SectorProcess(secaddr,u16l,u16m,u16r);
               //更新参数
               eaddr += u16m;
               esize -= u16m;
          }
     }

     return u16len;
}

/**
 * @brief DRI_Flash_EraseWriteData
 * Flash 阻塞型擦写数据函数(先擦除再写入)
 * 支持任意地址任意大小的写入
 * 如果地址和大小都是1024字节对齐，则栈空间不会被占用1024字节，否则栈空间会占用1024字节
 * @param wadd 写入地址
 * @param wd 待写入数据首地址
 * @param wl 待写入数据字节数
 * @return s32 小于0:写失败，其他值:实际写入字节数
 */
s32 DRI_Flash_EraseWriteData(u32 wadd,u8 *wd,u32 wl)
{
     s32 s32temp;

     if(wadd + wl > Flash_MaxSpaceSize)
     {//越界了
          return -1;
     }

     if(wl)
     {
     //先擦除
          s32temp = DRI_Flash_Erase(wadd,wl);
          if(s32temp != wl)
          {
               return s32temp;
          }
     //再写入
          return DRI_Flash_DirectWriteData(wadd,wd,wl);
     }
     return wl;
}

/**
 * @brief DRI_Flash_OutCapacity
 * 输出最大空间大小
 * @return u32 最大空间大小，单位字节数
 */
u32 DRI_Flash_OutCapacity(void)
{
     return Flash_MaxSpaceSize;
}

/********************************************************************************************************************************************
*                                                                                                                                           *
*               ----------------------------------以下是模块的内部函数代码区------------------------------------                          *
*                                                                                                                                           *
********************************************************************************************************************************************/
//待写入的地址必须是4字节对齐!!!
//返回实际写入的字节数
static u16 Flash_WriteData(u32 wadd,u8 *wd,u16 wl)
{
     u16 u16temp,u16num;

     fmc_unlock();

     u16num = (wl >> 2);
     for(u16temp = 0;u16temp < u16num;u16temp++)
     {
          if(FMC_READY == fmc_word_program(wadd,*(u32 *)wd))
          {
               wadd += 4;
               wd += 4;
               fmc_flag_clear(FMC_FLAG_BANK0_END);
               fmc_flag_clear(FMC_FLAG_BANK0_WPERR);
               fmc_flag_clear(FMC_FLAG_BANK0_PGERR);
          }
          else
          {
               fmc_flag_clear(FMC_FLAG_BANK0_END);
               fmc_flag_clear(FMC_FLAG_BANK0_WPERR);
               fmc_flag_clear(FMC_FLAG_BANK0_PGERR);
               break;
          }
     }
     
     fmc_lock();

     return (u16num << 2);
}

//纯扇区擦除
//secaddr为扇区地址，必须是扇区对齐
static s32 Flash_SectorErase(u32 secaddr)
{
     if(secaddr < Flash_SpaceStartAddr || secaddr > Flash_SpaceEndAddr)
     {
          return -1;
     }

     // unlock the flash program/erase controller 
     fmc_unlock();

     // clear all pending flags 
     fmc_flag_clear(FMC_FLAG_BANK0_END);
     fmc_flag_clear(FMC_FLAG_BANK0_WPERR);
     fmc_flag_clear(FMC_FLAG_BANK0_PGERR);

     fmc_page_erase(((secaddr >> 10) << 10));

     fmc_flag_clear(FMC_FLAG_BANK0_END);
     fmc_flag_clear(FMC_FLAG_BANK0_WPERR);
     fmc_flag_clear(FMC_FLAG_BANK0_PGERR);

     // lock the main FMC after the erase operation 
     fmc_lock();

     return Flash_MinSectorSize;
}

/**
 * @brief 
 * 扇区处理，主要用于半扇区擦除时，还有半扇区数据需要在擦除后再写入
 * @param addr 扇区起始地址(要求扇区对齐)
 * @param ldata 扇区前部读取字节数(可以为0)
 * @param mfl 扇区中部填充0xff字节数(不能为0)
 * @param rdata 扇区后部读取字节数(可以为0)
 */
static void Flash_SectorProcess(u32 addr,u16 ldata,u16 mfl,u16 rdata)
{
     u8 buff[Flash_MinSectorSize];
     u8 *rp = (u8*)addr;
     u16 u16i = 0;

     //整理数据
     for(;u16i < ldata;)
     {
          buff[u16i++] = *(u8 *)rp;
          rp++;
     }
     for(;u16i < (ldata + mfl);)
     {
          buff[u16i++] = 0xff;
          rp++;
     }
     for(;u16i < (ldata + mfl + rdata);)
     {
          buff[u16i++] = *(u8 *)rp;
          rp++;
     }
     //擦除扇区
     Flash_SectorErase(addr);//擦除扇区
     //写扇区数据
     Flash_WriteData(addr,buff,Flash_MinSectorSize);
}


